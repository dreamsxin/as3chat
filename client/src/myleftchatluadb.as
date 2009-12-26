package {
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.system.Security;
	
	import org.aswing.AsWingConstants;
	import org.aswing.AsWingManager;
	import org.aswing.BorderLayout;
	import org.aswing.Container;
	import org.aswing.FlowLayout;
	import org.aswing.GeneralListCellFactory;
	import org.aswing.JButton;
	import org.aswing.JFrame;
	import org.aswing.JLabel;
	import org.aswing.JList;
	import org.aswing.JOptionPane;
	import org.aswing.JPanel;
	import org.aswing.JScrollPane;
	import org.aswing.JSplitPane;
	import org.aswing.JTextArea;
	import org.aswing.JTextField;
	import org.aswing.JWindow;
	import org.aswing.VectorListModel;
	import org.aswing.event.AWEvent;
	import org.myleft.RoomListCell;
	import org.myleft.UserListCell;
	import org.myleft.core.SocketConnection;
	import org.myleft.data.Define;
	import org.myleft.data.User;
	import org.myleft.events.SocketConnectionEvent;

	public class myleftchatluadb extends Sprite
	{
		private var socket:SocketConnection;
		private var host:String = 'myleft.org';
		private var port:int = 8333;
		private var autologin:int;
		private var timestr:String;
		private var account:String;
		private var sign:String;
		private var myinfo:User;
		private var userJlist:JList
		private var usermodel:VectorListModel;
		
		private var roomJlist:JList
		private var roommodel:VectorListModel;
		
		private var mainWindow:JWindow;
		
		private var loginDialog:LoginJFrame;
		private var progressDialog:progressJFrame;
		
		private var chatText:JTextArea;
		private var mychatText:JTextArea;
		private var messageText:JTextField;
		private var toText:JTextField;
		private var sendButton:JButton;
		
		private var time:Date;
		
		private var mysound:mySound;
		public function myleftchatluadb()
		{
			AsWingManager.initAsStandard(this);
			
			mysound = new mySound();
			
			time = new Date;
			
			var param:Object = root.loaderInfo.parameters;
			autologin = int(param['autologin']);
			myinfo = new User;
			
			myinfo.username = param['username'] ? param['username'] : '';

			account = param['account']? param['account'] : '';
			sign = param['sign']? param['sign'] : '';
			timestr = param['timestamp'] ? param['timestamp'] : '';

			Security.loadPolicyFile("xmlsocket://"+host+":"+port.toString());

			mainWindow = new JWindow();
		
			//得到窗口容器
			var pane:Container = mainWindow.getContentPane();
			pane.setLayout(new BorderLayout());
			
			//在线用户列表
			usermodel = new VectorListModel();
			userJlist = new JList(usermodel, new GeneralListCellFactory(UserListCell,false,false));
			
			var userJframe:JFrame = new JFrame(mainWindow, "在线用户列表");
			userJframe.setClosable(false);
			userJframe.setResizable(false);
			
			userJframe.setSizeWH(200, 200);
			userJframe.getContentPane().append(new JScrollPane(userJlist));
			userJframe.setLocationXY(stage.stageWidth-userJframe.width, 0);
			userJframe.show();
			
			//房间列表
			roommodel = new VectorListModel();
			roomJlist = new JList(roommodel, new GeneralListCellFactory(RoomListCell,false,false));
			
			var roomJframe:JFrame = new JFrame(mainWindow, "房间列表");
			roomJframe.setClosable(false);
			roomJframe.setResizable(false);
			
			var roomToolJPanel:JPanel = new JPanel();
			
			var getroomlistBtn:JButton = new JButton('更新列表');
			getroomlistBtn.pack();
			getroomlistBtn.addActionListener(getRoomList);
			roomToolJPanel.append(getroomlistBtn);
			
			var joinroomlistBtn:JButton = new JButton('进入房间');
			joinroomlistBtn.pack();
			joinroomlistBtn.addActionListener(joinRoom);
			roomToolJPanel.append(joinroomlistBtn);
			
			roomJframe.setSizeWH(200, 200);
			roomJframe.getContentPane().append(roomToolJPanel, BorderLayout.NORTH);
			roomJframe.getContentPane().append(new JScrollPane(roomJlist), BorderLayout.CENTER);
			roomJframe.setLocationXY(stage.stageWidth-roomJframe.width, userJframe.height);
			roomJframe.show();
			
			//中间部分
			chatText = new JTextArea;
			chatText.setWordWrap(true);
			chatText.setEditable(false);
			var scrollPane:JScrollPane = new JScrollPane(chatText);
			
			//私聊
			mychatText = new JTextArea;
			mychatText.setWordWrap(true);
			mychatText.setEditable(false);
			var scrollPane2:JScrollPane = new JScrollPane(mychatText);
			
			var centerSplitPanel:JSplitPane = new JSplitPane(AsWingConstants.VERTICAL, false, scrollPane, scrollPane2);
			
			pane.append(centerSplitPanel, BorderLayout.CENTER);
			
			
			
			
			
			//底端
			var mlabel:JLabel = new JLabel("输入消息");
			
			messageText = new JTextField;
			messageText.setWordWrap(true);
			messageText.setPreferredWidth(450);
			messageText.addActionListener(sendFun);
			
			sendButton = new JButton('发送');
			sendButton.pack();
			sendButton.addActionListener(sendFun);
			
			var clearButton:JButton = new JButton('清理屏幕');
			clearButton.pack();
			clearButton.addActionListener(clearFun);
			
			var quitButton:JButton = new JButton('退出');
			quitButton.pack();
			quitButton.addActionListener(quitFun);
			
			var toolPanel:JPanel = new JPanel(new FlowLayout);
			
			toolPanel.appendAll(mlabel, messageText, sendButton, clearButton, quitButton);
			pane.append(toolPanel, BorderLayout.SOUTH);
	
			
						
			//设置窗口大小为舞台大小
			mainWindow.setSizeWH(stage.stageWidth, stage.stageHeight);
			//显示窗口
			mainWindow.show();
			
			loginDialog = new LoginJFrame(mainWindow, "登陆", true);
			loginDialog.pack();
			loginDialog.usernameText.setText(myinfo.username);
			loginDialog.setLocationXY((stage.stageWidth-loginDialog.width)*0.5, (stage.stageHeight-loginDialog.height)*0.5);
			loginDialog.loginButton.addActionListener(loginClickHandle);
			loginDialog.anonymousButton.addActionListener(anonymousClickHandle);
			loginDialog.connectButton.addActionListener(connectClickHandle);
			
			
			//进度条
			progressDialog = new progressJFrame(mainWindow, "连接中");
			progressDialog.pack();
			progressDialog.setLocationXY((stage.stageWidth-progressDialog.width)*0.5, (stage.stageHeight-progressDialog.height)*0.5);
			
			
			socket = new SocketConnection;
			socket.addEventListener(Define.CONNECT, eventHandle);
			socket.addEventListener(Define.CLOSE, eventHandle);
			socket.addEventListener(Define.EV_TYPE_AUTH, eventHandle);
			socket.addEventListener(Define.EV_TYPE_AUTH_OTHER_LOGIN, eventHandle);
			socket.addEventListener(Define.EV_TYPE_USER_LOGIN, eventHandle);
			socket.addEventListener(Define.EV_TYPE_USER_LOGOUT, eventHandle);
			socket.addEventListener(Define.EV_TYPE_USER_ADD, eventHandle);
			socket.addEventListener(Define.EV_TYPE_MESSAGE, eventHandle);
			socket.addEventListener(Define.EV_TYPE_CHANGE_ROOM, eventHandle);
			socket.addEventListener(Define.EV_TYPE_ROOM_ADD, eventHandle);

			if (autologin==1)
			{
				socket.connect(host, port);
				progressDialog.show();
			}
			else
			{
				loginDialog.show();
			}
			stage.addEventListener(Event.RESIZE, resizeHandler);
		}
		
		private function eventHandle(e:SocketConnectionEvent):void
		{
			trace('eventHandle1', e.type);
			trace('eventHandle2', e.body.toString());
			progressDialog.hide();
			var temStr:String;
			switch(e.type)
			{
				case Define.CONNECT:
					loginDialog.setConnectEnabled(false);
					loginDialog.setLoginEnabled(true);
					if (autologin==1)
					{
						loginFun();
					}
					break;
				case Define.CLOSE:
					usermodel.clear();
					loginDialog.setConnectEnabled(true);
					loginDialog.setLoginEnabled(false);
					loginDialog.show();
					break;
				case Define.EV_TYPE_USER_LOGIN:
					trace("进入聊天室", e.body.attributes.username, myinfo.username);
					if (e.body.attributes.username!=myinfo.username) {
						mysound.play(mySound.online);
						chatText.setHtmlText( '<font color="#FF9900"><b>' + e.body.attributes.username + '</b></font> 进入聊天室' + "<br/>" + chatText.getHtmlText());
					}
				case Define.EV_TYPE_USER_ADD:
					usermodel.append(new User(e.body.attributes.username));
					trace("append");
					break;
				case Define.EV_TYPE_USER_LOGOUT:
					chatText.setHtmlText( '<font color="#FF9900"><b>' + e.body.attributes.username + '</b></font> 离开聊天室' + "<br/>" + chatText.getHtmlText());
					for (var i:int=0;i < usermodel.getSize(); i++)
					{
						if (User(usermodel.get(i)).username == e.body.attributes.username)
						{
							usermodel.remove(usermodel.get(i));
							break;
						}
					}
					break;
				case Define.EV_TYPE_AUTH:
					if (e.body.attributes.result==1)
					{
						progressDialog.hide();
						loginDialog.hide();
						myinfo.username = e.body.attributes.username;
						usermodel.append(new User('所有人'));
						//JOptionPane.showMessageDialog("Hello", "登陆成功");
						getRoomList(null);
					}
					else
					{
						loginDialog.show();
						JOptionPane.showMessageDialog("Hello", e.body.attributes.message);
					}
					break;
				case Define.LOGIN_FAILURE:
					
					break;
				case Define.EV_TYPE_AUTH_OTHER_LOGIN:
					usermodel.clear();
					loginDialog.show();
					JOptionPane.showMessageDialog("Hello", e.body.attributes.message);
					break;
				case Define.EV_TYPE_MESSAGE:
					var ismy:Boolean = myinfo.username==e.body.attributes.from? true : false;
					var istomy:Boolean = myinfo.username==e.body.attributes.to? true : false;
					var toString:String =e.body.attributes.to ? ' 对 <b>'+ e.body.attributes.to + '</b>' : '';
					var date:Date = new Date(int(e.body.attributes.timestamp) * 1000);

					var dateStr:String = "    (" + date.fullYear + "-" + date.month + "-" + date.day + " " + date.hours + ":" + date.minutes + ")";
					var msgStr:String = e.body.firstChild.firstChild.toString();

					if (ismy) 
					{
						msgStr = '<font color="#0066FF">' + msgStr + '</font>';
					}
						
					if (toString)
					{
						if (istomy) 
						{
							mysound.play(mySound.msg);
						}

						mychatText.setHtmlText( '<font color="#FF6600"><b>' + e.body.attributes.from + '</b></font>'+ toString +' 说:' + msgStr + dateStr + "<br/>" + mychatText.getHtmlText());
					}
					else
					{
						chatText.setHtmlText( '<font color="#FF6600"><b>' + e.body.attributes.from + '</b></font> 说:' + msgStr + dateStr + "<br/>" + chatText.getHtmlText());
					}
					break;
				case Define.EV_TYPE_CHANGE_ROOM:
					chatText.setHtmlText('<font color="#FF9900"><b>欢迎您来到'+e.body.attributes.name+'</font>');
					usermodel.clear();
					break;
				case Define.EV_TYPE_ROOM_ADD:
					roommodel.append(Object({roomid:e.body.attributes.roomid, name:e.body.attributes.name}));
					break;
				
			}
		}
		
		//清理
		private function clearFun(e:AWEvent):void
		{
			mychatText.setHtmlText("");
			chatText.setHtmlText("");
		}
		
		//连接
		private function connectClickHandle(e:AWEvent):void
		{
			if (!socket.socket.connected) {
				progressDialog.setTitle('连接中...');
				progressDialog.show();
				socket.connect(host, port);
			}
		}
		
		//监听
		private function loginClickHandle(e:AWEvent):void
		{
			myinfo.username = loginDialog.usernameText.getText();
			loginFun();
		}
		
		//匿名登陆
		private function anonymousClickHandle(e:AWEvent):void
		{
			progressDialog.setTitle('登陆中...');
			progressDialog.show();
			//socket.send("<event type='100' anonymous='1'/>");
			socket.send("<event type='"+Define.EV_TYPE_AUTH+"' anonymous='1'/>");
		}
		
		//登陆
		private function loginFun():void
		{
			if (socket.socket.connected && myinfo.username.length>0)
			{
				progressDialog.setTitle('登陆中...');
				progressDialog.show();
				var temp:String="<event type='"+Define.EV_TYPE_AUTH+"' ";
				if (myinfo.username.length>0) temp += "username='"+myinfo.username+"' ";
				if (loginDialog.passwordText.getText().length>0) temp += "password='"+loginDialog.passwordText.getText()+"' ";
				if (timestr.length>0) temp += "timestamp='"+timestr+"' ";
				if (sign.length>0) temp += "sign='"+sign+"'";
				if (account.length>0) temp += "account='"+account+"' ";
				temp = temp+"/>";
				socket.send(temp);
			}
			else
			{
				JOptionPane.showMessageDialog("Hello", "请输入用户名");
			}
		}
		
		//获取房间列表
		private function getRoomList(e:AWEvent):void
		{
			if (socket.socket.connected) 
			{
				roommodel.clear();
				var toStr:String = userJlist.getSelectedValue() && userJlist.getSelectedValue().username!='所有人' ? "to='"+User(userJlist.getSelectedValue()).username+"'" : '';
				socket.send("<event type='"+Define.EV_TYPE_GET_ROOM_LIST+"'/>");
			}
		}
		
		//进入房间
		private function joinRoom(e:AWEvent):void
		{
			if (socket.socket.connected) 
			{
				if (roomJlist.getSelectedValue())
				{
					var obj:Object = roomJlist.getSelectedValue();
					socket.send("<event type='"+Define.EV_TYPE_CHANGE_ROOM+"' roomid='"+obj.roomid+"'/>");
				}
			}
			
		}
		
		private function quitFun(e:Event):void
		{
			if (socket.socket.connected) socket.send('<quit/>');
		}
		
		private function sendFun(e:AWEvent):void
		{
			if (messageText.getText().length>256)
			{
				 JOptionPane.showMessageDialog("Hello", "消息太长不能超过256个字符");
			}
			else if (socket.socket.connected && messageText.getText().length>0) 
			{
				var toStr:String = userJlist.getSelectedValue() && userJlist.getSelectedValue().username!='所有人' ? "to='"+User(userJlist.getSelectedValue()).username+"'" : '';
				socket.send("<event type='"+Define.EV_TYPE_MESSAGE+"' "+toStr+"><body><![CDATA["+messageText.getText()+"]]></body></event>");
				messageText.setText("");
			}
		}
		
		private function resizeHandler(e:Event):void
		{
			loginDialog.setLocationXY((stage.stageWidth-loginDialog.width)*0.5, (stage.stageHeight-loginDialog.height)*0.5);
			mainWindow.setSizeWH(stage.stageWidth, stage.stageHeight);
		}
	}
}
