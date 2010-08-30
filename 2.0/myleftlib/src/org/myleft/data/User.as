package org.myleft.data
{
	public class User
	{
		public var id:int;
		public var username:String;
		public var avatar:String;
		public var role:int;
		public var type:int;
		public function User(username:String='')
		{
			this.id = 0;
			this.avatar = '';
			this.role = 0;
			this.type = 0;
			this.username = username;
		}
		
		public function toString():String{
			return this.username;
		}

	}
}