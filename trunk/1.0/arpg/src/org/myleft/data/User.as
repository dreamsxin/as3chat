package org.myleft.data
{
	import as3isolib.display.IsoSprite;
	
	import com.gskinner.motion.GTween;
	
	import org.myleft.geom.IntPoint;
	
	public class User
	{
		public var username:String;
		public var point:IntPoint;
		public var nextpoint:IntPoint;
		public var display:IsoSprite;
		public var solution:Array;
		public var gt:GTween;
		public function User(username:String='', display:IsoSprite=null, x:int=1, y:int=1)
		{
			this.gt = new GTween;
			this.username = username;
			this.display = display;
			this.point = new IntPoint(x, y);
			this.nextpoint = new IntPoint(x, y);
			this.solution = [];
		}
		
		public function clear():void
		{
			this.point = new IntPoint(1, 1);
			this.nextpoint = new IntPoint(1, 1);
			this.solution = [];
		}
	}
}