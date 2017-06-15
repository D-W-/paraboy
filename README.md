# paraboy
伞兵多点安全认证系统


##登录

-  一个Sprite登录, 给服务端发一个消息Register(id)
-  服务器确认信息, 并群发信息以在所有的客户端创建一个新的Sprite createSprite(location)
-  客户端收到服务器群发消息, 在某一位置(服务器确定)创建一个Sprite

##移动

-  当前Sprite随着触控移动, 并给服务器发消息Location(id)
-  服务器确认消息, 并群发这个Sprite的位置
-  客户端收到群发消息, 更新指定ID 的Sprite的位置

##身份认证

-  Sprite点击另一个Sprite, 会给服务器发消息identity(id), 请求确认身份
-  服务器定向转发这个消息到指定的Sprite
-  另一Sprite收到身份认证消息, 完成身份认证, 返回认证结果
-  服务器转发回原来的发送者
-  发送者的客户端弹个框?


##选指挥官

-  每个客户端有一个选举按钮, 点击就发送选举消息 elect(ID)
-  服务器完成选举, 给选举成功的人返回消息
-  选举成功的人当选(换个头像?)
-  可比较的列表
-  当前最大
-  比较次数
-  每次比完都要更新这三个值

##开密码箱

-  每个客户端有一个密码箱按钮, 点击申请开密码箱给服务器发消息 openBox(ID)
-  当服务器收到足够多开箱子消息的时候(维护一个队列?)完成解密操作, 给客户端发消息打开箱子
-  客户端弹窗..

{
id: string
action: string [login, move, auth, auth2]
msg: json
}

{
action: string [create, move, auth, auth2]
msg: json
}


login
{
    publicKey: ,
    id: ,
}

move
{
    x: ,
    y:
}

auth
{
    target: ,

}

create
{
    id: ,
    x: ,
    y: 
}

move
{
    id: ,
    x: ,
    y:
}

