package org.myleft.core
{
	import flash.events.IEventDispatcher;
	
	import org.myleft.data.Define;
	
	public final class MyleftServer extends SocketConnection
	{
		
		public var _username:String;
		public var _roomid:int;
		
		private var _authtype:String;		
		private var _status:String;
		public function MyleftServer()
		{
			super();
		}
		
		public function get status():String
		{
			return _status;
		}
		
		public function get authtype():String
		{
			return _authtype;
		}
		
		[Bindable]
		public function set username(value:String):void
		{
			_username = value;
		}
		
		public function get username():String
		{
			return _username;
		}
		
		public function get roomid():int
		{
			return _roomid;
		}
		
		public function auth(username:String, password:String, roomid:int = -1):void
		{
			_username = username;
			_roomid = roomid;
			_authtype = Define.AUTH_TYPE_AUTH;
			var xml:XML = new XML('<event/>');
			xml.@type = Define.EV_TYPE_AUTH;
			xml.@username = username;
			xml.@password = password;
			xml.@roomid = roomid.toString();
			this.send(xml.toXMLString());
		}
		
		public function anonymousAuth(roomid:int = -1):void
		{
			_roomid = roomid;
			_authtype = Define.AUTH_TYPE_ANONYMOUS;
			var xml:XML = new XML('<event/>');
			xml.@type = Define.EV_TYPE_AUTH;
			xml.@anonymous = '1';
			xml.@roomid = roomid.toString();
			this.send(xml.toXMLString());
		}
		
		public function signAuth(username:String, account:String, sign:String, timestamp:String, roomid:int = -1):void
		{
			_authtype = Define.AUTH_TYPE_SIGN;
			
			var xml:XML = new XML('<event/>');
			xml.@type = Define.EV_TYPE_AUTH;
			xml.@username = username;
			xml.@timestamp = timestamp;
			xml.@sign = sign;
			xml.@account = account;
			xml.@roomid = roomid.toString();
			this.send(xml.toXMLString());
		}
		
		//退出
		public function logout():void
		{
			var xml:XML = new XML('<quit/>');
			this.send(xml.toXMLString());
		}
		
		//获取房间列表
		public function getRoomList():void
		{
			var xml:XML = new XML('<event/>');
			xml.@type = Define.EV_TYPE_GET_ROOM_LIST;
			this.send(xml.toXMLString());
		}
		
		//进入房间
		public function joinRoom(roomid:int = 0):void
		{
			_roomid = roomid;
			var xml:XML = new XML('<event/>');
			xml.@type = Define.EV_TYPE_CHANGE_ROOM;
			xml.@roomid = roomid.toString();
			this.send(xml.toXMLString());
		}
		
		//离开房间
		public function leaveRoom():void
		{
			_roomid = -1;
			var xml:XML = new XML('<event/>');
			xml.@type = Define.EV_TYPE_LEAVE_ROOM;
			this.send(xml.toXMLString());
		}
		
		//发送公共消息
		public function sendPrivateMessage(toUsername:String, message:String):void
		{
			var xml:XML = new XML('<event/>');
			xml.@type = Define.EV_TYPE_PRIVATE_MESSAGE;
			xml.@to = toUsername;
			xml.body = message;
			this.send(xml.toXMLString());
		}
		
		//发送公共消息
		public function sendPublicMessage(message:String):void
		{
			var xml:XML = new XML('<event/>');
			xml.@type = Define.EV_TYPE_PUBLIC_MESSAGE;
			xml.body = message;
			this.send(xml.toXMLString());
		}
	}
}