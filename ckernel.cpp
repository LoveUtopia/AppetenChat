#include "ckernel.h"
#include "friendwidget.h"

#include <QTextCodec>
#include <QDebug>
#include <windows.h>
#include <QInputDialog>
CKernel::CKernel()
{
    initProtocolArray();
    m_chatDlg = new chatDialog;
    m_chatDlg->showNormal();
    m_friendList = new mDialog;
    connect(m_chatDlg,SIGNAL(SIG_sendRegisterData(QString,QString,QString)),this,SLOT(slot_sendRegisterData(QString,QString,QString)));
    connect(m_chatDlg,SIGNAL(SIG_sendLoginData(QString,QString)),this,SLOT(slot_sendLoginData(QString,QString)));
    connect(m_chatDlg,SIGNAL(SIG_closeEvent()),this,SLOT(slot_closeEvent()));
    //绑定下线信号
    connect(m_friendList,SIGNAL(SIG_offline()),this,SLOT(slot_offline()));
    //绑定添加好友实现
    connect(m_friendList,SIGNAL(SIG_addFriend()),this,SLOT(slot_addFriend()));
    //创建群聊连接
    connect(m_friendList,SIGNAL(SIG_createGroup()),this,SLOT(slot_createGroup()));
    //添加群聊连接
    connect(m_friendList,SIGNAL(SIG_addGroup()),this,SLOT(slot_addGroup()));


}

CKernel::~CKernel()
{
    if(m_NetMediator){
        m_NetMediator->closeInet();
        delete m_NetMediator;
        m_NetMediator = nullptr;
    }
    if(m_chatDlg){
        m_chatDlg->hide();
        delete m_chatDlg;
        m_chatDlg = nullptr;
    }
    if(m_friendList){
        m_friendList->hide();
        delete m_friendList;
        m_friendList = nullptr;
    }
    //回收窗口
    for(auto ite = m_mapIdToChat.begin();ite!=m_mapIdToChat.end();){
        Chat* ch = *ite;
        if(ch){
            ch->hide();
            delete ch;
            ch = nullptr;
        }
        ite = m_mapIdToChat.erase(ite);
    }
}

void CKernel::initProtocolArray()
{
    memset(m_pfun, 0, sizeof(m_pfun));
    m_pfun[_DEF_TCP_LOGIN_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRs;
    m_pfun[_DEF_TCP_REGISTER_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRs;
    m_pfun[_DEF_TCP_FRIEND_INFO - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealFriendInfo;
    m_pfun[_DEF_TCP_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
    m_pfun[_DEF_TCP_CHAT_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRs;
    m_pfun[_DEF_TCP_OFFLINE_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
    m_pfun[_DEF_TCP_ADD_FRIEND_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
    m_pfun[_DEF_TCP_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
    m_pfun[_DEF_TCP_DELETE_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealDeleteFriendRs;
    m_pfun[_DEF_TCP_CREATE_GROUP_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealCreateGroupRs;
    m_pfun[_DEF_TCP_GROUP_INFO - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealGroupInfo;
    m_pfun[_DEF_TCP_GROUP_CHAT_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealGroupChatRq;
    m_pfun[_DEF_TCP_ADD_GROUP_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddGroupRq;
    m_pfun[_DEF_TCP_ADD_GROUP_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddGroupRs;
    m_pfun[_DEF_TCP_MEMBER_INFO - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealMemberInfo;
}

void CKernel::utf8ToGb2312(QString utf8, char *gb, int len)
{
    QTextCodec* gb2312 = QTextCodec::codecForName("gb2312");
    QByteArray ba = gb2312->fromUnicode(utf8);
    strcpy_s(gb,len,ba.data());
}

QString CKernel::gb2312ToUtf8(char *gb)
{
    QTextCodec* gb2312 = QTextCodec::codecForName("gb2312");
    return gb2312->toUnicode(gb);
}

void CKernel::dealLoginRs(char *data, int len, long from)
{
    //拆包
    STRU_TCP_LOGIN_RS* rq = (STRU_TCP_LOGIN_RS*)data;
    qDebug()<<rq->result;
    switch (rq->result) {
    case login_success:
    {   m_chatDlg->hide();
        m_friendList->showNormal();
        m_id = rq->id;
        break;
    }
    case login_password_error:
        QMessageBox::about(m_chatDlg,"错误","密码不正确");
        break;
    case login_tel_not_exist:
        QMessageBox::about(m_chatDlg,"错误","号码不存在");
        break;
    default:
        break;
    }
}

void CKernel::dealRegisterRs(char *data, int len, long from)
{
    STRU_TCP_REGISTER_RS* rq = (STRU_TCP_REGISTER_RS*)data;
    switch(rq->result){
    case register_success:
         QMessageBox::about(m_chatDlg,"注册提示","注册成功");
        break;
    case register_name_repeat:
        QMessageBox::about(m_chatDlg,"注册提示","注册失败，昵称重复");
    case register_tel_repeat:
        QMessageBox::about(m_chatDlg,"注册提示","注册失败，号码重复");
    default:
        break;

    }

    qDebug()<<rq->result;
    delete[] data;
}

void CKernel::dealFriendInfo(char *data, int len, long from)
{

    qDebug()<<__func__; 
    STRU_TCP_FRIEND_INFO* info = (STRU_TCP_FRIEND_INFO*)data;
    //转编码
    QString name = gb2312ToUtf8(info->name);
    QString feeling = gb2312ToUtf8(info->feeling);
    //1.判断是否为自身id
    if(m_id == info->friendId){
        //2.加载个人信息
        m_friendList->setSelfInfo(name,feeling,info->iconId);
        m_name = name;
        return;
    }
    //3.判断是否已经在列表上了
    if(m_mapIdToFriendwidget.count(info->friendId)){
        //4.已经在了，更新信息
        friendWidget* widget = m_mapIdToFriendwidget[info->friendId];
        widget->setInfo(info->friendId,info->iconId,name,feeling,info->status);
    }else{
        //5.不在列表，new一个窗口
        friendWidget* widget = new friendWidget;
        widget->setInfo(info->friendId,info->iconId,name,feeling,info->status);
        //将好友显示到列表上
        m_friendList->addFriend(widget);
        //将好友加入到map中
        m_mapIdToFriendwidget[info->friendId] = widget;
        connect(widget,SIGNAL(SIG_showChat(int)),this,SLOT(slot_showChatWidget(int)));
        connect(widget,SIGNAL(SIG_deleteFriend(int)),this,SLOT(slot_deleteFriend(int)));
    }
    //new对话框
    Chat* chat = new Chat;
    //设置聊天框信息
    chat->setChatInfo(name,info->friendId);
    //加入到map中
    m_mapIdToChat[info->friendId] = chat;
    qDebug()<<info->friendId;
    //绑定
    connect(chat,SIGNAL(SIG_chatMessage(QString,int)),this,SLOT(slot_chatMessage(QString,int)));
}

void CKernel::dealGroupInfo(char *data, int len, long from)
{
    qDebug()<<__func__;
    STRU_TCP_GROUP_INFO* info = (STRU_TCP_GROUP_INFO*)data;
    //转编码
    QString name = gb2312ToUtf8(info->groupName);
    //3.判断是否已经在列表上了
    if(m_mapIdToGroupItem.count(info->groupId)>0){
        //4.已经在了，更新信息
        groupItem* item = m_mapIdToGroupItem[info->groupId];
        item->setInfo(info->iconId,name,info->groupId);
    }else{

        //5.不在列表，new一个窗口
        groupItem* item = new groupItem;
        item->setInfo(info->iconId,name,info->groupId);
        //将好友显示到列表上
        m_friendList->addGroup(item);
        //将好友加入到map中
        m_mapIdToGroupItem[info->groupId] = item;
        connect(item,SIGNAL(SIG_showChatGroup(int)),this,SLOT(slot_showChatGroup(int)));
    }
    //new对话框
    ChatGroup* chat = new ChatGroup;
    //设置聊天框信息
    chat->setChatInfo(info->groupId,name);
    //加入到map中
    m_mapIdToChatGroup[info->groupId] = chat;

    //绑定
    connect(chat,SIGNAL(SIG_chatGroupMessage(QString,int)),this,SLOT(slot_chatGroupMessage(QString,int)));

}

void CKernel::dealChatRq(char *data, int len, long from)
{
    qDebug()<<__func__;
    //拆包
    STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)data;

    //取出聊天框
    if(m_mapIdToChat.count(rq->userId)>0){

        Chat* ch = m_mapIdToChat[rq->userId];
        if(ch){
        ch->setChatContent(rq->content);
        ch->showNormal();
               }

    }
}

void CKernel::dealChatRs(char *data, int len, long from)
{
    //拆包
    qDebug()<<__func__;
    STRU_TCP_CHAT_RS* rs = (STRU_TCP_CHAT_RS*)data;

    //取出聊天框
    if(m_mapIdToChat.count(rs->friendId)>0){
        Chat* ch = m_mapIdToChat[rs->friendId];
        ch->setFriendOffline();
        ch->showNormal();
    }
}

void CKernel::dealOfflineRq(char* data,int len,long from)
{
    //拆包
    STRU_TCP_OFFLINE_RQ* rq = (STRU_TCP_OFFLINE_RQ*)data;
    //把头像设置成暗显
    if(m_mapIdToFriendwidget.count(rq->userId)>0){
        friendWidget* wdg = m_mapIdToFriendwidget[rq->userId];
        if(wdg){
            wdg->setOffline();
        }
    }
}

void CKernel::dealAddFriendRq(char *data, int len, long from)
{
    STRU_TCP_ADD_FRIEND_RQ* rq = (STRU_TCP_ADD_FRIEND_RQ*)data;

    //弹出询问窗口
    STRU_TCP_ADD_FRIEND_RS rs;
    QString str = QString("用户 【%1】 想添加您为好友，是否同意？").arg(rq->userName);
    if(QMessageBox::Yes == QMessageBox::question(m_friendList,"好友申请",str)){
        rs.result = add_friend_success;
    }else{
        rs.result = add_friend_user_refuse;
    }
    strcpy_s(rs.friendName,m_name.toStdString().c_str());
    rs.userId = rq->userId;
    rs.friendId = m_id;
    //回复给服务器
    m_NetMediator->sendData((char*)&rs,sizeof(rs),1);

}

void CKernel::dealAddFriendRs(char *data, int len, long from)
{
    STRU_TCP_ADD_FRIEND_RS* rs = (STRU_TCP_ADD_FRIEND_RS*)data;
    //根据结果提示用户
    QString friendName = gb2312ToUtf8(rs->friendName);
    switch(rs->result){
    case add_friend_success:
        QMessageBox::about(m_friendList,"提示",QString("添加[%1]好友成功").arg(rs->friendName));
        break;
    case add_friend_no_this_user:
        QMessageBox::about(m_friendList,"提示",QString("添加[%1]好友失败，无此人").arg(friendName));
        break;
    case add_friend_user_offline:
        QMessageBox::about(m_friendList,"提示",QString("添加[%1]好友失败，用户下线").arg(friendName));
        break;
    case add_friend_user_refuse:
        QMessageBox::about(m_friendList,"提示",QString("添加[%1]好友失败，拒绝了你").arg(rs->friendName));
        break;
    }
}

void CKernel::dealDeleteFriendRs(char *data, int len, long from)
{
    //拆包
    STRU_TCP_DELETE_FRIEND_RS* rs = (STRU_TCP_DELETE_FRIEND_RS*)data;
    //是不是自己发过去的
    if(rs->userId == m_id){
        //是自己删除他的
        if(m_mapIdToFriendwidget.count(rs->friendId)>0){
            friendWidget* widget = m_mapIdToFriendwidget[rs->friendId];
            if(widget){
                QString str = QString("删除[%1]成功！").arg(widget->name());
                QMessageBox::about(m_friendList,"删除好友",str);
                //从map里删除
                if(m_mapIdToChat.count(rs->friendId)){
                    Chat* ch = m_mapIdToChat[rs->friendId];
                    ch->hide();
                    delete ch;
                    ch = nullptr;
                    m_mapIdToChat.erase(m_mapIdToChat.find(rs->friendId));
                }
                //隐藏
                widget->hide();
                delete widget;
                widget = nullptr;
                //从map里删除
                m_mapIdToFriendwidget.erase(m_mapIdToFriendwidget.find(rs->friendId));
            }
        }
    }
    else{
        //是被他人删除的
        if(m_mapIdToFriendwidget.count(rs->userId)>0){
            friendWidget* widget = m_mapIdToFriendwidget[rs->userId];
            if(widget){
                QString str = QString("您已被[%1]删除").arg(widget->name());
                QMessageBox::about(m_friendList,"删除好友",str);
                //从map里删除
                if(m_mapIdToChat.count(rs->userId)){
                    Chat* ch = m_mapIdToChat[rs->userId];
                    ch->hide();
                    delete ch;
                    ch = nullptr;
                    m_mapIdToChat.erase(m_mapIdToChat.find(rs->userId));
                }
                //隐藏
                widget->hide();
                delete widget;
                widget = nullptr;
                //从map里删除
                m_mapIdToFriendwidget.erase(m_mapIdToFriendwidget.find(rs->userId));
            }
        }
    }
}

void CKernel::dealCreateGroupRs(char *data, int len, long from)
{
    //拆包
    qDebug()<<__func__;
    STRU_TCP_CREATE_GROUP_RS* rs = (STRU_TCP_CREATE_GROUP_RS*)data;
    //判断结果
    if(rs->result == _create_group_success){
        QMessageBox::about(m_friendList,"提示","创建群聊成功!");
        return;
    }else if(rs->result == _create_group_name_repeat){
        QMessageBox::about(m_friendList,"错误","已有该群名");
        return;
    }
}

void CKernel::dealGroupChatRq(char *data, int len, long from)
{
    qDebug()<<__func__;
    //拆包
    STRU_TCP_GROUP_CHAT_RQ* rq = (STRU_TCP_GROUP_CHAT_RQ*)data;
    //判断是不是自己发的
    if(rq->userId!=m_id){
    //取出聊天框
    if(m_mapIdToChatGroup.count(rq->groupId)>0){
        ChatGroup* ch = m_mapIdToChatGroup[rq->groupId];
        if(ch){
        ch->setChatContent(rq->content,rq->name);
        ch->showNormal();
               }

    }
    }
}

void CKernel::dealAddGroupRq(char *data, int len, long from)
{
    STRU_TCP_ADD_GROUP_RQ* rq = (STRU_TCP_ADD_GROUP_RQ*)data;

    //弹出询问窗口
    STRU_TCP_ADD_GROUP_RS rs;
    QString groupName = gb2312ToUtf8(rq->groupName);
    QString str = QString("用户 【%1】 想添加入群聊[%2]，是否同意？").arg(rq->userName).arg(groupName);
    if(QMessageBox::Yes == QMessageBox::question(m_friendList,"入群申请",str)){
        rs.result = add_group_success;
    }else{
        rs.result = add_group_leader_refuse;
    }
    strcpy_s(rs.groupName,rq->groupName);
    rs.userId = rq->userId;
    //回复给服务器
    m_NetMediator->sendData((char*)&rs,sizeof(rs),1);
}

void CKernel::dealAddGroupRs(char *data, int len, long from)
{
    STRU_TCP_ADD_GROUP_RS* rs = (STRU_TCP_ADD_GROUP_RS*)data;
    //根据结果提示用户
    QString groupName = gb2312ToUtf8(rs->groupName);
    switch(rs->result){
    case add_group_success:
        QMessageBox::about(m_friendList,"提示",QString("添加[%1]群聊成功").arg(groupName));
        break;
    case add_group_no_this_group:
        QMessageBox::about(m_friendList,"提示",QString("添加[%1]群聊失败，无此人").arg(groupName));
        break;
    case add_group_leader_offline:
        QMessageBox::about(m_friendList,"提示",QString("添加[%1]群聊失败，用户下线").arg(groupName));
        break;
    case add_group_leader_refuse:
        QMessageBox::about(m_friendList,"提示",QString("添加[%1]群聊失败，拒绝了你").arg(groupName));
        break;
    }
}

void CKernel::dealMemberInfo(char *data, int len, long from)
{
    qDebug()<<__func__;
    STRU_TCP_MEMBER_INFO* info = (STRU_TCP_MEMBER_INFO*)data;
    //转编码
    QString name = gb2312ToUtf8(info->name);
    QString feeling = gb2312ToUtf8(info->feeling);

    //3.判断是否已经在列表上了
    if(m_mapIdToChatGroup.count(info->groupId)){
        ChatGroup* ch = m_mapIdToChatGroup[info->groupId];
        if(ch){
            if(ch->m_mapIdToFriendwidget.count(info->friendId)){
                friendWidget* widget = m_mapIdToFriendwidget[info->friendId];
                widget->hideDeleteUi();
                widget->setInfo(info->friendId,info->iconId,name,feeling,info->status);
            }else{
                //5.不在列表，new一个窗口
                friendWidget* widget = new friendWidget;
                widget->setInfo(info->friendId,info->iconId,name,feeling,info->status);
                widget->hideDeleteUi();
                //将好友显示到列表上
                ch->addMember(widget);
                //将好友加入到map中
                ch->m_mapIdToFriendwidget[info->friendId] = widget;
                connect(widget,SIGNAL(SIG_showChat(int)),this,SLOT(slot_showChatWidget(int)));
                connect(widget,SIGNAL(SIG_deleteFriend(int)),this,SLOT(slot_deleteFriend(int)));
            }
        }
        //4.已经在了，更新信息

    }else{

    }
}

void CKernel::slot_openInet()
{
    m_NetMediator = new TcpClientMediator(m_chatDlg->getIp());
    connect(m_NetMediator,SIGNAL(SIG_dealData(char*,int,long)),this,SLOT(slot_dealData(char*,int,long)));
    if(!m_NetMediator->openInet()){
        QMessageBox::about(m_chatDlg,"error","open error");
        exit(0);
    }
}

void CKernel::slot_dealData(char *data, int len, long from)
{
    qDebug()<<__func__;
    //取出协议头
        packType type = *(packType*)data;
        qDebug()<<type;
        type = type - _DEF_PROTOCOL_BASE - 1;
        //判断合法性是否越界
        if (type > 0 && type < _DEF_PROTOCOL_COUNT) {
            PFun func = m_pfun[type];
            if (func) {
                (this->*func)(data,len,from);
            }
            else {
                //1.结构体初始化出错 2.初始化数组出问题
                qDebug() << "type error" << type << endl;
            }
        }
        else {
            //1.结构体初始化出错 2.offset未置零
            qDebug() << "type error:" << type << endl;
        }
}

void CKernel::slot_sendRegisterData(QString name,QString tel, QString psd)
{
    qDebug()<<__func__ << "name" << name;
    STRU_TCP_REGISTER_RQ rq;
    utf8ToGb2312(name,rq.name,sizeof(rq.name));
    qDebug()<<__func__ << "name" << rq.name;
    strcpy_s(rq.tel,tel.toStdString().c_str());
    strcpy_s(rq.password,psd.toStdString().c_str());

    //连接网络
    slot_openInet();
    //发送给服务端kernel

    m_NetMediator->sendData((char*)&rq,sizeof(rq),1);
}

void CKernel::slot_sendLoginData(QString tel, QString psd)
{
    isOpen = true;
    qDebug()<<__func__;
    STRU_TCP_LOGIN_RQ rq;
    strcpy_s(rq.tel,tel.toStdString().c_str());
    strcpy_s(rq.password,psd.toStdString().c_str());
    //连接网络
    slot_openInet();
    //发送给服务端kernel

    m_NetMediator->sendData((char*)&rq,sizeof(rq),1);
}

void CKernel::slot_showChatWidget(int friendId)
{
    qDebug()<<__func__;
    if(m_mapIdToChat.count(friendId)>0){
        Chat* chat = m_mapIdToChat[friendId];
        if(chat){
            chat->showNormal();
        }
    }
}

void CKernel::slot_chatMessage(QString content, int friendId)
{
    //打包
    STRU_TCP_CHAT_RQ rq;
    strcpy_s(rq.content,content.toStdString().c_str());
    rq.friendId = friendId;
    rq.userId = m_id;

    //发送给客户端
    m_NetMediator->sendData((char*)&rq,sizeof(rq),1);
}

void CKernel::slot_closeEvent()
{
    qDebug()<<__func__;
    if(m_NetMediator){
        m_NetMediator->closeInet();
        delete m_NetMediator;
        m_NetMediator = nullptr;
    }

    if(m_chatDlg){
        m_chatDlg->hide();
        delete m_chatDlg;
        m_chatDlg = nullptr;
    }
    if(m_friendList){
        m_friendList->hide();
        delete m_friendList;
        m_friendList = nullptr;
    }
    //回收窗口
    for(auto ite = m_mapIdToChat.begin();ite!=m_mapIdToChat.end();){
        Chat* ch = *ite;
        if(ch){
            ch->hide();
            delete ch;
            ch = nullptr;
        }
        ite = m_mapIdToChat.erase(ite);
    }

}

void CKernel::slot_offline()
{
    //给服务端发下线请求
    STRU_TCP_OFFLINE_RQ rq;
    rq.userId = m_id;
    m_NetMediator->sendData((char*)&rq,sizeof(rq),1);

    //回收
    slot_closeEvent();
}

void CKernel::slot_addFriend()
{
    qDebug()<<__func__;
    QString friendName = QInputDialog::getText(m_friendList,"添加好友","请输入昵称");
    QString nameTemp = friendName;
    //校验合法性
    if(friendName.isEmpty()||nameTemp.remove(" ").isEmpty()||friendName.length()>_DEF_NAME_MAX){
        QMessageBox::about(m_friendList,"错误","名字无效");
        return;
    }
    //判断是否是自己
    if(m_name == friendName){
        QMessageBox::about(m_friendList,"错误","名字为自己");
        return;
    }
    //判断是不是已经是好友了
    for(auto ite = m_mapIdToFriendwidget.begin();ite!=m_mapIdToFriendwidget.end();ite++){
        friendWidget* it = *ite;
        if(it){
            if(friendName == it->name()){
                QMessageBox::about(m_friendList,"错误","已添加好友");
                return;
            }
        }
    }

    //打包发送

    STRU_TCP_ADD_FRIEND_RQ rq;
    //strcpy_s(rq.friendName,friendName.toStdString().c_str());
    rq.userId = m_id;
    utf8ToGb2312(friendName,rq.friendName,sizeof(rq.friendName));
    strcpy_s(rq.userName,m_name.toStdString().c_str());
    m_NetMediator->sendData((char*)&rq,sizeof(rq),1);
}

void CKernel::slot_deleteFriend(int m_friendId)
{
   //向服务器发送删除好友请求
   qDebug()<<__func__;
   STRU_TCP_DELETE_FRIEND_RQ rq;
   rq.friendId = m_friendId;
   rq.userId = m_id;

   m_NetMediator->sendData((char*)&rq,sizeof(rq),1);
}

void CKernel::slot_createGroup()
{
    qDebug()<<__func__;
    QString groupName = QInputDialog::getText(m_friendList,"创建群聊","输入群昵称");
    QString nameTemp = groupName;
    //校验合法性
    if(groupName.isEmpty()||nameTemp.remove(" ").isEmpty()||groupName.length()>_DEF_NAME_MAX){
        QMessageBox::about(m_friendList,"错误","名字无效");
        return;
    }
    //判断是不是已经加了这个群聊了
    for(auto ite = m_mapIdToGroupItem.begin();ite!=m_mapIdToGroupItem.end();ite++){
        groupItem* it = *ite;
        if(it){
            if(groupName == it->name()){
                QMessageBox::about(m_friendList,"错误","已添加该群");
                return;
            }
        }
    }

    //打包发送
    STRU_TCP_CREATE_GROUP_RQ rq;
    rq.userId = m_id;
    utf8ToGb2312(groupName,rq.groupName,sizeof(rq.groupName));
    m_NetMediator->sendData((char*)&rq,sizeof(rq),1);

}

void CKernel::slot_showChatGroup(int groupId)
{
    qDebug()<<__func__;
    if(m_mapIdToChatGroup.count(groupId)>0){
        ChatGroup* chat = m_mapIdToChatGroup[groupId];
        if(chat){
            chat->showNormal();
        }
    }
}

void CKernel::slot_chatGroupMessage(QString content, int groupId)
{
    //打包
    STRU_TCP_GROUP_CHAT_RQ rq;
    strcpy_s(rq.content,content.toStdString().c_str());
    rq.groupId = groupId;
    rq.userId = m_id;
    strcpy_s(rq.name,m_name.toStdString().c_str());

    //发送给客户端
    m_NetMediator->sendData((char*)&rq,sizeof(rq),1);
}

void CKernel::slot_addGroup()
{
    qDebug()<<__func__;
    QString groupName = QInputDialog::getText(m_friendList,"创建群聊","输入群昵称");
    QString nameTemp = groupName;
    //校验合法性
    if(groupName.isEmpty()||nameTemp.remove(" ").isEmpty()||groupName.length()>_DEF_NAME_MAX){
        QMessageBox::about(m_friendList,"错误","名字无效");
        return;
    }
    //判断是不是已经加了这个群聊了
    for(auto ite = m_mapIdToGroupItem.begin();ite!=m_mapIdToGroupItem.end();ite++){
        groupItem* it = *ite;
        if(it){
            if(groupName == it->name()){
                QMessageBox::about(m_friendList,"错误","已添加该群");
                return;
            }
        }
    }

    //打包发送
    STRU_TCP_ADD_GROUP_RQ rq;
    rq.userId = m_id;
    strcpy_s(rq.userName,m_name.toStdString().c_str());
    utf8ToGb2312(groupName,rq.groupName,sizeof(rq.groupName));
    m_NetMediator->sendData((char*)&rq,sizeof(rq),1);
}


