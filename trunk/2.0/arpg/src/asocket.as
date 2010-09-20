package {
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.system.Security;
	
	import org.aswing.AsWingManager;
	import org.aswing.BorderLayout;
	import org.aswing.Container;
	import org.aswing.FlowLayout;
	import org.aswing.JButton;
	import org.aswing.JLabel;
	import org.aswing.JOptionPane;
	import org.aswing.JPanel;
	import org.aswing.JScrollPane;
	import org.aswing.JTextArea;
	import org.aswing.JTextField;
	import org.aswing.JWindow;
	import org.aswing.event.AWEvent;
	import org.myleft.core.SocketConnection;
	import org.myleft.data.Define;
	import org.myleft.events.SocketConnectionEvent;

	public class myleftchat extends Sprite
	{
		private var socket:SocketConnection;
		
		private var mainWindow:JWindow;
		
		private var loginDialog:LoginJFrame;
		
		private var chatText:JTextArea;
		private var messageText:JTextField;
		private var toText:JTextField;
		private var sendButton:JButton;
		public function myleftchatlua()
		{
			AsWingManager.initAsStandard(this);

			Security.loadPolicyFile("xmlsocket://myleft.org:6222");

			mainWindow = new JWindow();
		
			//得到窗口容器
			var pane:Container = mainWindow.getContentPane();
			pane.setLayout(new BorderLayout());
			
			//顶端
			
			//pane.append(toolPanel, BorderLayout.NORTH);
			
			//中间部分
			chatText = new JTextArea;
			
			var scrollPane:JScrollPane = new JScrollPane(chatText);			
			pane.append(scrollPane, BorderLayout.CENTER);
			
			//底端
			var mlabel:JLabel = new JLabel("输入消息");
			
			messageText = new JTextField;
			messageText.setWordWrap(true);
			messageText.setPreferredWidth(384);
			messageText.addActionListener(sendFun);
			
			var tolabel:JLabel = new JLabel("接收人");
			
			toText = new JTextField("all");
			toText.setPreferredWidth(60);
			
			sendButton = new JButton('发送');
			sendButton.pack();
			sendButton.addActionListener(sendFun);
			
			var quitButton:JButton = new JButton('退出');
			quitButton.pack();
			quitButton.addActionListener(quitFun);
			
			var toolPanel:JPanel = new JPanel(new FlowLayout);

			toolPanel.appendAll(mlabel, messageText, tolabel, toText, sendButton, quitButton);
			pane.append(toolPanel, BorderLayout.SOUTH);
	
			
						
			//设置窗口大小为舞台大小
			mainWindow.setSizeWH(stage.stageWidth, stage.stageHeight);
			//显示窗口
			mainWindow.show();
			
			loginDialog = new LoginJFrame(mainWindow, "登陆", true);
			loginDialog.pack();
			loginDialog.setLocationXY((stage.stageWidth-loginDialog.width)*0.5, (stage.stageHeight-loginDialog.height)*0.5);
			loginDialog.loginButton.addActionListener(loginClickHandle);
			loginDialog.connectButton.addActionListener(connectClickHandle);
			loginDialog.show();
			
			socket = new SocketConnection;
			socket.addEventListener(Event.CONNECT, initFun);
			socket.addEventListener(Event.CLOSE, closeFun);
			socket.addEventListener(Define.LOGIN_SUCCESS, eventHandle);
			socket.addEventListener(Define.LOGIN_FAILURE, eventHandle);
			socket.addEventListener(Define.PUBLIC_MESSAGE, eventHandle);
			socket.addEventListener(Define.EV_TYPE_AUTH_OTHER_LOGIN, eventHandle);

		
			stage.addEventListener(Event.RESIZE, resizeHandler);
		}
		
		private function eventHandle(e:SocketConnectionEvent):void
		{
			trace(e.type);
			trace(e.body.toString());
			switch(e.type)
			{
				case Define.LOGIN_SUCCESS:
					loginDialog.hide();
					JOptionPane.showMessageDialog("Hello", "登陆成功");
					break;
				case Define.LOGIN_FAILURE:
					loginDialog.show();
				case Define.EV_TYPE_AUTH_OTHER_LOGIN:
					trace("Define.EV_TYPE_AUTH_OTHER_LOGIN");
					loginDialog.show();
					JOptionPane.showMessageDialog("Hello", e.body.attributes.message);
					break;
				case Define.PUBLIC_MESSAGE:
					chatText.setText( e.body.attributes.from + ":" + e.body.firstChild.firstChild.toString() + "\n" + chatText.getText());
					break;
			}
		}
		
		//连接
		private function connectClickHandle(e:AWEvent):void
		{
			if (!socket.socket.connected) socket.connect(loginDialog.hostText.getText(), int(loginDialog.portText.getText()));
		}
		
		//监听
		private function loginClickHandle(e:AWEvent):void
		{
			if (loginDialog.usernameText.getText().length>0)
			{
				if (socket.socket.connected) socket.send("<event type='"+Define.EV_TYPE_AUTH+"'><username>"+loginDialog.usernameText.getText()+"</username><password>"+loginDialog.passwordText.getText()+"</password></event>");
			}
			else
			{
				JOptionPane.showMessageDialog("Hello", "请输入用户名");
			}
		}
		
		private function resizeHandler(e:Event):void
		{
			loginDialog.setLocationXY((stage.stageWidth-loginDialog.width)*0.5, (stage.stageHeight-loginDialog.height)*0.5);
			mainWindow.setSizeWH(stage.stageWidth, stage.stageHeight);
		}
		
		private function initFun(e:Event):void
		{
			loginDialog.setConnectEnabled(false);
			loginDialog.setLoginEnabled(true);
		}
		
		private function quitFun(e:Event):void
		{
			if (socket.socket.connected) socket.send('<quit/>');
		}
		
		private function closeFun(e:Event):void
		{
			loginDialog.setConnectEnabled(true);
			loginDialog.setLoginEnabled(false);
			loginDialog.show();
		}
		
		private function sendFun(e:AWEvent):void
		{
			if (messageText.getText().length>256)
			{
				 JOptionPane.showMessageDialog("Hello", "消息太长不能超过256个字符");
			}
			else if (socket.socket.connected && messageText.getText().length>0) 
			{
				socket.send("<event type='"+Define.EV_TYPE_MESSAGE+"' to='"+toText.getText()+"'><body>"+messageText.getText()+"</body></event>");
				messageText.setText("");
			}
		}
	}
}
