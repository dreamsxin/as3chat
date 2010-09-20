package {

	import com.gskinner.motion.GTween;
	
	import flash.display.Sprite;
	
	import org.aswing.ASColor;
	import org.aswing.JLabel;
	import org.aswing.JTextArea;
	import org.aswing.border.LineBorder;
	import org.myleft.core.BitmapSprite;
	import org.myleft.data.CharacterStatic;

	public class Character extends Sprite
	{
		public var state:String=CharacterStatic.stand;
		
		private var sprite:BitmapSprite;
		
		private var standSprite:BitmapSprite = new BitmapSprite('assets/character/character_01.png', 88, 87, -44, -50);
		private var walkSprite:BitmapSprite = new BitmapSprite('assets/character/character_02.png', 88, 93, -44, -50);
		
		private var tile:BitmapSprite = new BitmapSprite('assets/tile/0.png', 100, 48);
		
		private var c:Sprite;
		private var label:JLabel;
		private var message:JTextArea;
		public function Character(username:String='')
		{
			tile.x = -tile.width/2;
			this.addChild(tile);
			sprite = standSprite;
			
			c = new Sprite;
			c.addChild(sprite);
			this.addChild(c);

			label = new JLabel(username);
			label.setForeground(new ASColor(0xff6600));
			label.setOpaque(true);
			label.setBackground(new ASColor(0xffffff));
			
			label.setBorder(new LineBorder());
			label.pack();
			label.setLocationXY(tile.x, 0 - tile.height);
			this.addChild(label);
			
			message = new JTextArea();
			message.setPreferredWidth(160);
			message.setWordWrap(true);
			message.alpha = 0;
			message.setBackground(new ASColor(0, 0));
			message.setForeground(new ASColor(0xff0000));
			//message.setOpaque(true);
			message.setBorder(new LineBorder());
			this.addChild(message);
			
		}
		
		public function stand():void
		{
			
			standSprite.Direction = sprite.Direction;
			
			c.removeChild(sprite);
			sprite = standSprite;
			c.addChild(sprite);
		}
		
		private var gt:GTween;
		public function talk(m:String):void
		{
			//trace('talk', m);
			message.setHtmlText(m);
			message.pack();
			message.setLocationXY(label.x, label.y);
			if (!gt) 
			{
				gt = new GTween();
			}
			gt.target = message;
			gt.duration = 2;
			gt.paused = true;
			gt.setValues({alpha:1, y:label.y - message.height});
			gt.onComplete = function(g:GTween):void{g.duration = 8; g.setValue('alpha', 0)};
			
		}
		
		public function walk():void
		{
			walkSprite.Direction = sprite.Direction;
			
			c.removeChild(sprite);
			sprite = walkSprite;
			c.addChild(sprite);
		}
		
		public function toggleDirection():void
		{
			if (sprite.Direction<=2)
			{
				sprite.Direction = sprite.Direction + 1;
			}
			else
			{
				sprite.Direction = 0;
			}
		}

		public function setDirection(v:int):void
		{
			if (v<=3 && v>=0) sprite.Direction = v;
		}

		public function getDirection():int
		{
			return sprite.Direction;
		}
	}
}
