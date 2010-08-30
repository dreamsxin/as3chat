package org.myleft
{
	import org.aswing.Component;
	import org.aswing.DefaultListCell;
	import org.aswing.JLabel;
	import org.aswing.event.ResizedEvent;
	import org.myleft.data.User;
	
	public class RoomListCell extends DefaultListCell
	{
	
		public function RoomListCell()
		{
			super();
		}
				
				
		override public function setCellValue(value:*) : void {
		    super.setCellValue(value);
		    getJLabel().setText(getStringValue(value.name));
		    __resized(null);
		}
	}
}