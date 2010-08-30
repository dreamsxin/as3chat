package org.myleft.events
{
	import flash.events.Event;
	import flash.xml.XMLNode;

	public class SocketConnectionEvent extends Event
	{		
		public var body:XMLNode = new XMLNode(3, "");
		public function SocketConnectionEvent(type:String, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super( type, false, false );
		}

		override public function toString():String
		{
			return '[SocketConnectionEvent type="' + type + '" bubbles=' + bubbles +
				' cancelable=' + cancelable + ' eventPhase=' + eventPhase + ']';
		}
	}
}
