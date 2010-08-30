package component
{
	import flash.display.Stage;
	
	import org.aswing.FlowLayout;
	import org.aswing.JButton;
	import org.aswing.JComboBox;
	import org.aswing.JPanel;
	import org.aswing.JTextField;

	public class ToolContainer extends JPanel
	{
		public var chatComboBox:JComboBox;
		public var messageText:JTextField;
		public var sendButton:JButton;
		public var quitButton:JButton;
		
		public var standButton:JButton;
		public var walkButton:JButton;
		public var toggleButton:JButton;
		
		public var upButton:JButton;
		public var downButton:JButton;
		public var leftButton:JButton;
		public var rightButton:JButton;
		public function ToolContainer()
		{
			super(new FlowLayout);
			
			//底端
			chatComboBox=new JComboBox();
			chatComboBox.setPreferredWidth(60);
			chatComboBox.setListData([
				'世界',
				'地图',
				'队友',
				'好友'
			]);
			chatComboBox.setSelectedIndex(0);
			
			messageText = new JTextField;
			messageText.setMaxChars(256);
			messageText.setWordWrap(true);
			messageText.setPreferredWidth(384);
			
			sendButton = new JButton('发送');
			sendButton.pack();
			
			quitButton = new JButton('退出');
			quitButton.pack();

			this.appendAll(chatComboBox, messageText, sendButton, quitButton);
			
			walkButton = new JButton('行走');
			toggleButton = new JButton('旋转');
			upButton = new JButton('上');
			downButton = new JButton('下');
			leftButton = new JButton('左');
			rightButton = new JButton('右');
			this.appendAll(walkButton, toggleButton, upButton, downButton, leftButton, rightButton);
		}
		
	}
}