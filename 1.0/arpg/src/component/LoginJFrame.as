package component
{
	import org.aswing.ASColor;
	import org.aswing.Component;
	import org.aswing.FlowLayout;
	import org.aswing.JButton;
	import org.aswing.JFrame;
	import org.aswing.JLabel;
	import org.aswing.JPanel;
	import org.aswing.JTextField;
	import org.aswing.ext.Form;

	public class LoginJFrame extends JFrame
	{
		private var loginForm:Form;
		public var usernameText:JTextField;
		public var passwordText:JTextField;
		public var hostText:JTextField;
		public var portText:JTextField;
		
		public var connectButton:JButton;
		public var loginButton:JButton;
		public var regButton:JButton;
		public function LoginJFrame(owner:Object=null, title:String='登陆', modal:Boolean=false)
		{
			super(owner, title, modal);
			this.setClosable(false);
			loginForm = new Form;
			usernameText = new JTextField("Demo");
			usernameText.setPreferredWidth(100);
			this.addLabelRow(usernameText, "用户名");
			
			passwordText = new JTextField;
			passwordText.setPreferredWidth(100);
			this.addLabelRow(passwordText, "密码");
			
			hostText = new JTextField("myleft.org");
			hostText.setPreferredWidth(100);
			this.addLabelRow(hostText, "主机");
			
			portText = new JTextField("8222");
			portText.setPreferredWidth(100);
			this.addLabelRow(portText, "端口");
			
			connectButton = new JButton("连接");
			
			loginButton = new JButton("登陆");
			loginButton.setEnabled(false);
			
			regButton = new JButton("注册");
			
			var buttonPane:JPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 16, 5));
			buttonPane.appendAll(connectButton, loginButton, regButton);
			loginForm.append(buttonPane);
			
			this.setContentPane(loginForm);
		}
		
		public function setConnectEnabled(b:Boolean):void
		{
			connectButton.setEnabled(b);
		}
		
		public function setLoginEnabled(b:Boolean):void
		{
			loginButton.setEnabled(b);
		}
		
		//添加一个FormRow，text为第一格标签，c为第二格的组件，toolTip不为null的话为第三格标签
		private function addLabelRow(c:Component, text:String, toolTip:String=null):void{
			var label:JLabel = new JLabel(text, null, JLabel.LEFT);
			var tip:JLabel = null;
			if(toolTip){
				tip = new JLabel(toolTip, null, JLabel.LEFT);
				tip.setForeground(ASColor.GRAY);
			}
			loginForm.addRow(label, c, tip);
		}
		
	}
}