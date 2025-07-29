#include "../../include/ChatService.hpp"
#include "../../include/public.hpp"

ChatService *ChatService::getInstance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    _msghandleMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2)});
    _msghandleMap.insert({REG_MSG, std::bind(&ChatService::registe, this, _1, _2)});
    _msghandleMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2)});
    _msghandleMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2)});
    _msghandleMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2)});
    _msghandleMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2)});
    _msghandleMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2)});
    _msghandleMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2)});
}

msghandle &ChatService::GetHandle(int msgType)
{
    return _msghandleMap[msgType];
}

void ChatService::login(const TcpConnectionPtr &conn, json &js)
{
    // 获取json登录信息
    int id = js["id"].get<int>();
    string passwd = js["passwd"];

    // 登录验证
    User user = _userModel.query(id);
    if (user.getId() == id && user.getPwd() == passwd)
    {
        if (user.getState() == "online")
        {
            json respond;
            respond["msqid"] = LOGIN_MSG_ACK;
            respond["error"] = 2;
            respond["msg"] = "这个账户已登录!";
            conn->send(respond.dump() + "\n");
        }
        else
        {
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({user.getId(), conn});
            }

            user.setState("online");
            _userModel.updateState(user);

            json respond;
            respond["msqid"] = LOGIN_MSG_ACK;
            respond["error"] = 0;
            respond["msg"] = "登录成功!";

            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty())
            {
                respond["offlinemsg"] = vec;
                // 读取该用户的离线消息后，把该用户的所有离线消息删除掉
                _offlineMsgModel.remove(id);
            }

            // 查询该用户的好友信息并返回
            vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty())
            {
                vector<string> vec2;
                for (User &user : userVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                respond["friends"] = vec2;
            }

            // 查询用户的群组信息
            vector<Group> groupuserVec = _groupModel.queryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }
                respond["groups"] = groupV;
            }

            conn->send(respond.dump() + "\n");
        }
    }
    else
    {
        json respond;
        respond["msqid"] = LOGIN_MSG_ACK;
        respond["error"] = 1;
        respond["msg"] = "登录失败。密码错误或者用户不存在!";
        conn->send(respond.dump() + "\n");
    }
}

void ChatService::registe(const TcpConnectionPtr &conn, json &js)
{
    User user;
    user.setName(js["name"]);
    user.setPwd(js["passwd"]);

    if (_userModel.insert(user))
    {
        // 注册成功
        json respond;
        respond["msgid"] = REG_MSG;
        respond["error"] = 0;
        respond["id"] = user.getId();
        conn->send(respond.dump() + "\n");
    }
    else
    {
        // 注册失败
        json respond;
        respond["msgid"] = REG_MSG;
        respond["error"] = 1;
        conn->send(respond.dump() + "\n");
    }
}

void ChatService::loginout(const TcpConnectionPtr &conn, json &js)
{
    int userid = js["id"].get<int>();
    User user;
    {
        lock_guard<mutex> lock(_connMutex);

        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
             _userConnMap.erase(it);
        }

        user.setId(userid);
        user.setState("offline");
        _userModel.updateState(user);
    }
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 从map表删除用户的链接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 更新用户的状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js)
{
    int toid = js["toid"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // toid在线，转发消息   服务器主动推送消息给toid用户
            it->second->send(js.dump() + "\n");
            return;
        }
    }

    // toid不在线，存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

void ChatService::reset()
{
    _userModel.resetState();
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js)
{
     int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 存储好友信息
    if (_friendModel.insert(userid, friendid))
    {
        json respond;
        respond["error"] = 0;
        respond["msg"] = "添加好友成功!";
        conn->send(respond.dump() + "\n");
    }
    else
    {
        json respond;
        respond["error"] = 1;
        respond["msg"] = "添加好友失败!";
        conn->send(respond.dump() + "\n");
    }
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group))
    {
        // 存储群组创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}

// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(_connMutex);
    for (int id : useridVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 转发群消息
            it->second->send(js.dump());
        }
        else
        {
            // 存储离线群消息
            _offlineMsgModel.insert(id, js.dump());
        }
    }
}
