package org.myleft.core
{
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.TimerEvent;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.net.URLRequest;
	import flash.system.ApplicationDomain;
	import flash.system.LoaderContext;
	import flash.utils.Timer;

	public class BitmapSprite extends Sprite
	{
		public var playTimer:Timer;
		
		private var bitmapcontext:LoaderContext = new LoaderContext();
		private var loader:Loader;
		private var allBitmap:Bitmap;
		private var allBitmapData:BitmapData;
		
		private var unitBitmap:Bitmap;
		private var unitBitmapData:BitmapData;
		private var unitWidth:int;
		private var unitHeight:int;
		private var unitRect:Rectangle;
		
		private var unitVerticalNum:int=1;
		private var unitFrameNum:int;
		private var currentFrame:int=2;
		private var direction:int;
		
		private var unitX:int;
		private var unitY:int;
		
		private var ht:Shape;
		
		public function BitmapSprite(url:String, w:int=80, h:int=91, x:int=0, y:int=0)
		{
			super();
			this.ht = new Shape;
			
			this.unitWidth = w;
			this.unitHeight = h;
			
			this.unitX = x;
			this.unitY = y;
			
			this.direction = 0;
			
			this.bitmapcontext.applicationDomain = ApplicationDomain.currentDomain;
			loader = new Loader;
			loader.load(new URLRequest(url), bitmapcontext);
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, this.complete);
			
			//playTimer = new Timer(100);
			//playTimer.addEventListener(TimerEvent.TIMER, timerPlay);
		}
		
		private function complete(e:Event):void
		{
			loader.contentLoaderInfo.removeEventListener(Event.COMPLETE, this.complete);
			var bitmap:Bitmap = Bitmap(e.target.loader.content);
			this.allBitmapData = bitmap.bitmapData.clone();
			loader.unload();
			
			this.initBitmap();
		}
		
		private function initBitmap():void
		{
			this.unitBitmapData = new BitmapData(this.unitWidth, this.unitHeight,true,0x00000000);
			
			this.unitBitmap = new Bitmap(this.unitBitmapData);
			this.unitBitmap.x = unitX;
			this.unitBitmap.y = unitY;
			
			this.unitRect = new Rectangle(0,0,this.unitWidth,this.unitHeight);
			
			this.unitFrameNum = allBitmapData.width/this.unitWidth;
			this.unitVerticalNum = allBitmapData.height/this.unitHeight;
			
			this.unitBitmapData.copyPixels(this.allBitmapData, this.unitRect, new Point(0,0));
			
			this.addChild(this.unitBitmap);
			//var matrix:Matrix = new Matrix();
			//this.unitBitmapData.draw(this.allBitmapData, matrix, null, null, this.unitRect);
			
			if (this.unitFrameNum>0) 
			{
				//playTimer.start();
				this.addEventListener(Event.ENTER_FRAME, play);
			}
		}
		
		private function play(event:Event):void
		{
			this.gotoAndPlay();
		}
		
		private function timerPlay(event:TimerEvent):void
		{
			this.gotoAndPlay();
		}
		
		/**
		 * 移动到下一个frame
		 */
		public function gotoAndPlay():void
		{
			//取当前帧
			if (this.currentFrame == this.unitFrameNum)	//该方向上最后一帧，回到开始
			{
				this.currentFrame = -1;
			}
			this.currentFrame = this.currentFrame + 1;
			
			//根据frame和方向取动作图片
			this.unitRect.x = this.currentFrame * this.unitWidth;	//在原图片中的像素位置
			
			this.direction = this.direction%this.unitVerticalNum;
			this.unitRect.y = this.direction * this.unitHeight;		//在原图片中的像素位置
			
			//拷贝该frame的图片
			this.unitBitmapData.copyPixels(this.allBitmapData, this.unitRect, new Point(0,0));
		}
		
		//属性
		override public function get width():Number
		{
			return this.unitWidth;
		}
		override public function get height():Number
		{
			return this.unitHeight;
		}
		
		public function get Direction():int
		{
			return this.direction;
		}
		
		public function set Direction(direction:int):void
		{
			this.direction=direction;
		}
		
		public function set setCurrentFrame(frame:int):void
		{
			this.currentFrame=frame;
		}
	}
}