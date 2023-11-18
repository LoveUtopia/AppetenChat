#ifndef CKERNEL_H
#define CKERNEL_H

#include "inetMediator/TcpClientMediator.h"
#include "chatdialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QObject>
#include "inet/packDef.h"
#include "mdialog.h"
#include <QMap>
#include "chat.h"
#include "chatgroup.h"
class CKernel;
typedef void (CKernel::*PFun)(char* data, int len, long from);
class CKernel:public QObject
{
   Q_OBJECT
public:
    CKernel();
    ~CKernel();
    void initProtocolArray();
    void utf8ToGb2312(QString utf8,char* gb,int len);
    QString gb2312ToUtf8(char* gb);
    void dealLoginRs(char* data,int len,long from);
    void dealRegisterRs(char* data,int len,long from);
    void dealFriendInfo(char* data,int len,long from);
    void dealGroupInfo(char* data,int len,long from);
    //处理聊天请求
    void dealChatRq(char* data,int len,long from);
    //处理聊天回复
    void dealChatRs(char* data,int len,long from);
    //处理下线请求
    void dealOfflineRq(char* data,int len,long from);
    //处理添加好友请求
    void dealAddFriendRq(char* data, int len, long from);
    //处理添加好友回复
    void dealAddFriendRs(char* data, int len, long from);
    //处理删除好友回复
    void dealDeleteFriendRs(char* data, int len, long from);
    //处理创建群聊回复
    void dealCreateGroupRs(char* data, int len, long from);
    //处理群聊天请求
    void dealGroupChatRq(char* data,int len,long from);
    //处理添加群聊请求
    void dealAddGroupRq(char* data, int len, long from);
    //处理添加群聊回复
    void dealAddGroupRs(char* data, int len, long from);
    //处理群成员信息函数
    void dealMemberInfo(char* data, int len, long from);
    bool isOpen = false;
    int m_id;
    QString m_name;
    INetMediator* m_NetMediator = nullptr;
    chatDialog* m_chatDlg = nullptr;
    PFun m_pfun[_DEF_PROTOCOL_COUNT];
    mDialog* m_friendList = nullptr;
    QMap<int,friendWidget*> m_mapIdToFriendwidget;
    //对话框map
    QMap<int,Chat*> m_mapIdToChat;
    //群聊map
    QMap<int,groupItem*> m_mapIdToGroupItem;
    //群聊对话框map
    QMap<int,ChatGroup*> m_mapIdToChatGroup;


public slots:
    void slot_openInet();
    void slot_dealData(char* data,int len,long from);
    void slot_sendRegisterData(QString name,QString tel, QString psd);

    void slot_sendLoginData(QString tel, QString psd);
    //显示聊天窗口
    void slot_showChatWidget(int friendId);
    //处理聊天信息
    void slot_chatMessage(QString content,int friendId);
    //登陆界面关闭
    void slot_closeEvent();
    //处理下线
    void slot_offline();
    //按下添加好友
    void slot_addFriend();
    //删除好友
    void slot_deleteFriend(int m_friendId);
    //创建群聊
    void slot_createGroup();
    //显示群聊
    void slot_showChatGroup(int groupId);
    //处理群聊信息
    void slot_chatGroupMessage(QString content,int groupId);
    //处理添加群聊
    void slot_addGroup();
};

#endif // CKERNEL_H
