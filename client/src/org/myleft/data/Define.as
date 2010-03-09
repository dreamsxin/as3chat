package org.myleft.data
{
	public class Define
	{
		public static const CONNECT:String = "CONNECT";
		public static const ACTIVATE:String = "ACTIVATE";
		
		public static const CLOSE:String = "CLOSE";
		public static const NODEFINE:String = "NODEFINE";
		
		public static const LOGIN_SUCCESS:String = "LOGIN_SUCCESS";
		public static const LOGIN_FAILURE:String = "LOGIN_FAILURE";
		public static const PUBLIC_MESSAGE:String = "PUBLIC_MESSAGE";
		
		public static const EV_TYPE_PING:String = '0';
		
		public static const EV_TYPE_AUTH:String = '10';
		public static const EV_TYPE_AUTH_OTHER_LOGIN:String = '11';
		
		public static const EV_TYPE_USER_LOGIN:String = '20';
		public static const EV_TYPE_USER_LOGOUT:String = '21';
		public static const EV_TYPE_USER_ADD:String = '22';
		
		public static const EV_TYPE_MESSAGE:String = '30';
		
		public static const EV_TYPE_CHANGE_ROOM:String = '40';
		public static const EV_TYPE_GET_ROOM_LIST:String = '41';
		public static const EV_TYPE_ROOM_ADD:String = '42';
		
		public static const EV_TYPE_ADMIN_COMMAND_GOOUT:String = '90';
		public static const EV_TYPE_ADMIN_COMMAND_SHUTUP:String = '91';
	}
}