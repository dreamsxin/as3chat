package
{
	import mx.core.ClassFactory;
	import as3isolib.display.IsoSprite;
	import as3isolib.display.IsoView;
	import as3isolib.display.primitive.IsoBox;
	import as3isolib.display.renderers.*;
	import as3isolib.display.scene.IsoGrid;
	import as3isolib.display.scene.IsoScene;
	import as3isolib.geom.IsoMath;
	import as3isolib.geom.Pt;
	import as3isolib.graphics.SolidColorFill;
	import as3isolib.graphics.Stroke;
	
	import com.gskinner.motion.GTween;
	
	import component.LoginJFrame;
	import component.MiniMap;
	import component.PlayerStatusContainer;
	import component.ToolContainer;
	
	import eDpLib.events.ProxyEvent;
	
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.net.URLLoader;
	import flash.net.URLRequest;
	
	import org.aswing.ASColor;
	import org.aswing.AsWingManager;
	import org.aswing.CursorManager;
	import org.aswing.GeneralListCellFactory;
	import org.aswing.JFrame;
	import org.aswing.JList;
	import org.aswing.JOptionPane;
	import org.aswing.JPanel;
	import org.aswing.JScrollPane;
	import org.aswing.JTextArea;
	import org.aswing.JWindow;
	import org.aswing.UIManager;
	import org.aswing.VectorListModel;
	import org.aswing.event.AWEvent;
	import org.myleft.UserListCell;
	import org.myleft.astar.AStar;
	import org.myleft.core.MyleftServer;
	import org.myleft.data.CharacterStatic;
	import org.myleft.data.Define;
	import org.myleft.data.TileMap;
	import org.myleft.data.User;
	import org.myleft.events.SocketConnectionEvent;
	import org.myleft.geom.IntPoint;
	
	import skins.ALookAndFeel;

	[SWF(backgroundColor="#869ca7")]//,frameRate="30"
	public class arpg extends Sprite
	{
		private var server:MyleftServer;
		
		private var param:Object;
		private var host:String = 'myleft.org';
		private var port:int = 8888;
		private var userJlist:JList
		private var model:VectorListModel = new VectorListModel;
		
		private var myinfo:User;
		private var time:Date;
		
		//游戏视窗
		private var minimap:MiniMap;
		private var floorScene:IsoScene;
		private var mainScene:IsoScene;
		private var frontScene:IsoScene;

		//游戏对象
		private var view:IsoView;
		private var rectBox:IsoBox;
		private var heroBox:IsoSprite;
		private var boxfills:Array=[new SolidColorFill(0xff0000, .5), new SolidColorFill(0x00ff00, .5), new SolidColorFill(0x0000ff, .5), new SolidColorFill(0xff0000, .5), new SolidColorFill(0x00ff00, .5), new SolidColorFill(0x0000ff, .5)];

		//地图数据
		private var tileSize:int=50;
		private var tileMap:TileMap;

		//当前所在位置
		private var currentPt:IntPoint;

		private var myGTween:GTween;
		
		//控制视窗
		private var mainWindow:JWindow;
		private var topContainer:JPanel;//顶部容器		
		private var playerStatusContainer:PlayerStatusContainer;//状态栏容器
		private var toolContainer:ToolContainer;//工具栏容器
		
		private var scrollPane:JScrollPane;
		private var chatText:JTextArea;
		private var loginDialog:LoginJFrame;
		private var progressDialog:progressJFrame;
		
		private var mysound:mySound;
		private var xmlLoader:URLLoader;

		public function arpg()
		{
			AsWingManager.initAsStandard(this);
			CursorManager.getManager().showCustomCursor(CursorAnimation.defaultCursor);
			UIManager.setLookAndFeel(new ALookAndFeel());
			
			param = root.loaderInfo.parameters;
			if (param['host']) {
				host = param['host'];
			}
			if (param['port']) {
				port = param['port'];
			}
			
			myGTween=new GTween();
			
			this.cacheAsBitmap = true;
			
			currentPt=new IntPoint(1, 1);
			
			mysound = new mySound();
			
			loadmap("data/map01.xml");
			
			game_init();
			
			this.addChild(view);
			
			/** 状态栏 **/
			playerStatusContainer=new PlayerStatusContainer;
			playerStatusContainer.mouseEnabled = false;
			playerStatusContainer.validate();
			this.addChild(playerStatusContainer);
			
			//在线用户
			var userJframe:JFrame = new JFrame(this, "在线用户列表");
			userJframe.setClosable(false);
			userJframe.setResizable(false);
			
			userJlist = new JList(model, new GeneralListCellFactory(UserListCell,false,false));

			userJframe.setSizeWH(200, 300);
			userJframe.getContentPane().append(new JScrollPane(userJlist));
			userJframe.setLocationXY(0, playerStatusContainer.height);
			userJframe.show();
			
			myinfo = new User;
			
			/** 小地图 **/
			minimap = new MiniMap();
			minimap.mouseEnabled = false;
			minimap.x = stage.stageWidth - minimap.width;
			minimap.y = 0;
			this.addChild(minimap);

			/** 工具栏 **/
			toolContainer=new ToolContainer();
			toolContainer.mouseEnabled=false;	
			toolContainer.setSizeWH(stage.stageWidth, 30);
			toolContainer.y = stage.stageHeight - toolContainer.height;
			toolContainer.validate();
			this.addChild(toolContainer);
			
			
			//聊天窗口
			chatText = new JTextArea;
			chatText.setWordWrap(true);
			chatText.setBackground(new ASColor(0, 0));
			chatText.setForeground(new ASColor(0xff0000));
			scrollPane = new JScrollPane(chatText);	
			scrollPane.mouseEnabled = false;
			scrollPane.setSizeWH(300, 150);
			scrollPane.validate();
			scrollPane.y = toolContainer.y - scrollPane.height;
			this.addChild(scrollPane);
			
			toolContainer.messageText.addActionListener(sendFun);
			toolContainer.sendButton.addActionListener(sendFun);
			toolContainer.quitButton.addActionListener(quitFun);
			
			toolContainer.walkButton.addActionListener(function(e:AWEvent):void
				{
					var p:IntPoint;
					switch (heroBox.sprites[0].getDirection())
					{
						case 0: //x
							p=screenToMap(heroBox.x + tileSize, heroBox.y);
							break;
						case 1: //y
							p=screenToMap(heroBox.x, heroBox.y + tileSize);
							break;
						case 2: //-x
							p=screenToMap(heroBox.x - tileSize, heroBox.y);
							break;
						case 3: //-y
							p=screenToMap(heroBox.x, heroBox.y - tileSize);
							break;
					}
					if (tileMap.isWalkable(p.x, p.y))
					{
						myinfo.solution=[];
						myinfo.solution.push(p);
						if (myGTween && myGTween.position < 1)
							return;
						timeStart(null, myinfo);
					}
				});
			toolContainer.toggleButton.addActionListener(function(e:AWEvent):void
				{
					heroBox.sprites[0].toggleDirection();
				});


			toolContainer.upButton.addActionListener(function(e:AWEvent):void
				{
					heroBox.sprites[0].setDirection(3);
				});
			toolContainer.downButton.addActionListener(function(e:AWEvent):void
				{
					heroBox.sprites[0].setDirection(1);
				});
			toolContainer.leftButton.addActionListener(function(e:AWEvent):void
				{
					heroBox.sprites[0].setDirection(2);
				});
			toolContainer.rightButton.addActionListener(function(e:AWEvent):void
				{
					heroBox.sprites[0].setDirection(0);
				});
				
			time = new Date;
			var username:String = '游客'+((Math.random()*1000)>>0)+time.getHours()+time.getSeconds()+time.getHours()
			
			loginDialog = new LoginJFrame(mainWindow, "登陆", true);
			loginDialog.setResizable(false);
			
			loginDialog.pack();
			loginDialog.usernameText.setText(username);
			loginDialog.setLocationXY((stage.stageWidth-loginDialog.width)*0.5, (stage.stageHeight-loginDialog.height)*0.5);
			loginDialog.loginButton.addActionListener(loginClickHandle);
			loginDialog.connectButton.addActionListener(connectClickHandle);
			loginDialog.hostText.setText(host.toString());
			loginDialog.portText.setText(port.toString());
	
			loginDialog.show();
			
			//进度条
			progressDialog = new progressJFrame(mainWindow, "连接中");
			progressDialog.pack();
			progressDialog.setLocationXY((stage.stageWidth-progressDialog.width)*0.5, (stage.stageHeight-progressDialog.height)*0.5);
			
			
			server = new MyleftServer;
			server.addEventListener(Define.CONNECT, eventHandle);
			server.addEventListener(Define.CLOSE, eventHandle);
			server.addEventListener(Define.EV_TYPE_AUTH_SUCCESS, eventHandle);
			server.addEventListener(Define.EV_TYPE_AUTH_FAILURE, eventHandle);
			server.addEventListener(Define.EV_TYPE_AUTH_OTHER_LOGIN, eventHandle);
			server.addEventListener(Define.EV_TYPE_USER_LOGIN, eventHandle);
			server.addEventListener(Define.EV_TYPE_USER_LOGOUT, eventHandle);
			server.addEventListener(Define.EV_TYPE_USER_ADD, eventHandle);
			server.addEventListener(Define.EV_TYPE_SYSTEM_MESSAGE, eventHandle);
			server.addEventListener(Define.EV_TYPE_PRIVATE_MESSAGE, eventHandle);
			server.addEventListener(Define.EV_TYPE_PUBLIC_MESSAGE, eventHandle);
			server.addEventListener(Define.EV_TYPE_CHANGE_ROOM, eventHandle);
			server.addEventListener(Define.EV_TYPE_ROOM_ADD, eventHandle);
			server.addEventListener(Define.EV_TYPE_MOVE, eventHandle);

			stage.addEventListener(Event.RESIZE, resizeHandler);
		}
		
		public function loadmap(url:String):void {
			
			if (!this.xmlLoader) this.xmlLoader = new URLLoader();
			this.xmlLoader.addEventListener(Event.COMPLETE, createData);
			this.xmlLoader.load(new URLRequest(url));

		}
		
		private function createData(e:Event):void {
			this.xmlLoader.removeEventListener(Event.COMPLETE, createData);
			XML.ignoreWhitespace = true; 
			var mapdata:XML = new XML(e.target.data);
			var building:XMLList = mapdata.building.item;

			if (mapdata.grid.attribute('tileSize')) this.tileSize = mapdata.grid.attribute('tileSize');

			//地图数据
			if (!tileMap) tileMap=new TileMap();
			tileMap.initialize(mapdata.grid.attribute('w'), mapdata.grid.attribute('h'));
			
			
			var a:IsoSprite=new IsoSprite();
			a.setSize(this.tileSize*2, this.tileSize*1/2, this.tileSize*1/2);
			a.moveTo(tileSize*3+10, tileSize*3+10, tileSize*1);
			a.sprites=[new Talisman()];
			mainScene.addChild(a);
			
			var pos:Array,size:Array;
			for each (var item:XML in building) {
				pos = item.attribute('pos').toString().split(',');
				size = item.attribute('size').toString().split(',');
				trace('walkable', item.attribute('walkable')!='0');
				tileMap.setWalkableRect(pos[0], pos[1], size[0], size[1], item.attribute('walkable')!='0');
				
				var b:IsoSprite=new IsoSprite();
				b.setSize(this.tileSize*size[0], this.tileSize*size[1], this.tileSize*size[2]);
				b.moveTo(tileSize*pos[0], tileSize*pos[1], tileSize*pos[2]);
				b.sprites=[Building[item.attribute('name')]];
				mainScene.addChild(b);
				trace(" item: "+ item.attributes().toString());
			}
			
			minimap.initialize(tileMap);
		}
		
		private function sendFun(e:AWEvent):void
		{
			if (toolContainer.messageText.getText().length>256)
			{
				 JOptionPane.showMessageDialog("Hello", "消息太长不能超过256个字符");
			}
			else if (toolContainer.messageText.getText().length>0) 
			{
				if (!userJlist.getSelectedValue() || userJlist.getSelectedValue().username=='所有人') {
					server.sendPublicMessage(toolContainer.messageText.getText());
				} else if (userJlist.getSelectedValue()) {
					server.sendPrivateMessage(User(userJlist.getSelectedValue()).username, toolContainer.messageText.getText());
				}
				toolContainer.messageText.setText("");
			}
		}
		
		private function quitFun(e:Event):void
		{
			server.logout();
		}
		
		private function eventHandle(e:SocketConnectionEvent):void
		{
			progressDialog.hide();
			//trace('eventHandle', e.type, e.body.toString());
			var o:XML = e.body;
			
			var msgStr:String;
			var date:Date;
			var dateStr:String;
			var i:int;
			if (o.attribute('timestamp').length()==1) {
				date = new Date(int(o.attribute('timestamp')) * 1000);
			} else {
				date = new Date();
			}
			dateStr = '<font color="#999999">[' + date.hours + ':' + date.minutes + ':' + date.seconds + ']</font>';
			
			var regex:RegExp = new RegExp("\\n", "gi");
			if (o.child('body').length()==1) {
				msgStr = o.body.toString();
				msgStr = msgStr.replace(regex, "<br>");
				//trace(msgStr);
			}

			switch(e.type)
			{
				case Define.CONNECT:
					loginDialog.setConnectEnabled(false);
					loginDialog.setLoginEnabled(true);
					break;
				case Define.EV_TYPE_AUTH_OTHER_LOGIN:
					this.mylogout();
					loginDialog.show();
					JOptionPane.showMessageDialog("Hello", o.message);
					break;
				case Define.CLOSE:
					this.mylogout();
					loginDialog.setConnectEnabled(true);
					loginDialog.setLoginEnabled(false);
					loginDialog.show();					
					break;
				case Define.EV_TYPE_USER_ADD:
				case Define.EV_TYPE_USER_LOGIN:
					var user:User = new User(o.attribute('username'), createSprite(o.attribute('username'), o.attribute('x'), o.attribute('y')));
					//trace("append", o.username, o.x, o.y);
					user.nextpoint = user.point = new IntPoint(o.attribute('x'), o.attribute('y'));
					
					
					if (myinfo.username == o.attribute('username'))
					{
						myinfo = user;
						heroBox = myinfo.display;
						
						minimap.drawRole(myinfo.point);
						this.view.centerOnIso(heroBox);
						this.view.render();
					}
					else
					{
						mysound.play(mySound.online);
						//trace("append2", o.username, o.x, o.y);
					}
					model.append(user);
					this.mainScene.addChild(user.display);
					this.mainScene.render();
					break;
				case Define.EV_TYPE_USER_LOGOUT://其他用户退出
					
					for (i=0;i < model.getSize(); i++)
					{
						if (User(model.get(i)).username == o.attribute('username'))
						{
							deleteUser( User(model.get(i)) );
							break;
						}
					}
					
					this.mainScene.render();
					break;
				case Define.EV_TYPE_AUTH_SUCCESS:					
					myinfo.username = o.attribute('username');
					server.username = o.attribute('username');
					mainScene.render();
					loginDialog.hide();
					this.mainScene.container.visible = true;
					JOptionPane.showMessageDialog("Hello", "登陆成功");
					break;
				case Define.EV_TYPE_AUTH_FAILURE:
					loginDialog.show();
					this.mainScene.container.visible = false;
					JOptionPane.showMessageDialog("Hello", o.message);
					break;
				case Define.EV_TYPE_SYSTEM_MESSAGE:					
					chatText.setHtmlText( '<b>系統消息：</b>' + msgStr + dateStr + "<br/>" + chatText.getHtmlText());
					break;
				case Define.EV_TYPE_PRIVATE_MESSAGE:
					var ismy:Boolean = false;
					var istomy:Boolean = false;
					if (myinfo.username==o.attribute('from')) {
						o.from = '<font color="#FF6600">【'+o.attribute('from')+'】</font>';
						ismy = true;
					}
					if (myinfo.username==o.to) {
						o.to = '<font color="#FF6600">【'+o.attribute('to')+'】</font>';
						istomy = true;
					}
					var toString:String =o.attribute('to') ? ' 對 <b>'+ o.attribute('to') + '</b>' : '';
	
					if (ismy) 
					{
						msgStr = '<font color="#0066FF">' + msgStr + '</font>';
					}					
					
					if (istomy && !ismy) 
					{
						mysound.play(mySound.msg);
					}
					
					chatText.setHtmlText( '<b>' + o.attribute('from') + '</b>'+ toString +' 說:' + msgStr + dateStr + "<br/>" + chatText.getHtmlText());
					for (i=0;i < model.getSize(); i++)
					{
						user = User(model.get(i));
						if (user.username == o.attribute('from'))
						{
							if (user.display) user.display.sprites[0].talk(msgStr);
							break;
						}
					}
					break;
				case Define.EV_TYPE_PUBLIC_MESSAGE:					
					chatText.setHtmlText( '<b>' + o.attribute('from') + '</b> 說:' + msgStr + dateStr + "<br/>" + chatText.getHtmlText());
				
					for (i=0;i < model.getSize(); i++)
					{
						user = User(model.get(i));
						if (user.username == o.attribute('from'))
						{
							if (user.display) user.display.sprites[0].talk(msgStr);
							break;
						}
					}
					break;
				case Define.EV_TYPE_MOVE:
					//trace("EV_TYPE_MOVE");
					for (i=0;i < model.getSize(); i++)
					{
						//trace('username'+User(model.get(i)).username, 'from'+o.attribute('from'));
						if (User(model.get(i)).username == o.attribute('from') && o.attribute('from')!=myinfo.username)
						{
							//trace('开始移动');
							User(model.get(i)).solution.push(new IntPoint(o.attribute('x'), o.attribute('y')));
							if (User(model.get(i)).gt && User(model.get(i)).gt.position<1 && User(model.get(i)).gt.position>0) return;
							timeStart(User(model.get(i)).gt, User(model.get(i)));
							//User(model.get(i)).display.moveTo(o.x * tileSize, o.y * tileSize, 0);
							//this.mainScene.render();
							break;
						}
					}
					break;
				
				case Define.EV_TYPE_CHANGE_ROOM:
					chatText.setHtmlText('<font color="#FF9900">歡迎您進入【'+o.attribute('name')+'】</font>');
					model.clear();
					model.append(new User('所有人'));
					break;
			}
		}
		
		//连接
		private function connectClickHandle(e:AWEvent):void
		{
			progressDialog.setTitle('连接中...');
			progressDialog.show();
			server.connect(loginDialog.hostText.getText(), int(loginDialog.portText.getText()));
		}
		
		//监听
		private function loginClickHandle(e:AWEvent):void
		{
			progressDialog.setTitle('登陆中...');
			progressDialog.show();
			if (loginDialog.usernameText.getText().length>0 && loginDialog.passwordText.getText().length>0)
			{
				server.auth(loginDialog.usernameText.getText(), loginDialog.passwordText.getText(), 0);
			}
			else
			{
				server.anonymousAuth(0);
			}
		}
		//发送位置
		private function sendMove(x:int, y:int):void
		{
			server.move(x, y);
		}
		
		//用户退出
		private function mylogout():void
		{
			this.rectBox.moveTo(0, 0, 0);
			this.floorScene.render();

			myinfo.gt.paused = true;
			myinfo.clear();
			while (model.getSize())
			{
				deleteUser( User(model.get(0)) );
			}
			model.clear();
			//this.mainScene.removeAllChildren();
			this.mainScene.render();
			this.mainScene.container.visible = false;
		}
		
		private function deleteUser(user:User):void
		{
			var obj:IsoSprite;
			obj = IsoSprite(user.display);
			obj.removeAllChildren();
			this.mainScene.removeChild(obj);
				
			obj = null;
			model.remove(user);

			user = null;
		}
		
		//初始化
		private function game_init():void
		{

			/** 视窗 **/
			view=new IsoView();
			view.mouseEnabled = false;
			//view.autoUpdate = true;
			view.clipContent=true; //遮罩
			//view.viewRenderers = [new ClassFactory(DefaultViewRenderer)];//,new ClassFactory(ViewBoundsRenderer)
			//view.setSize(300, 400);
			view.setSize(stage.stageWidth, stage.stageHeight);

			/** 场景 地板 **/
			floorScene=new IsoScene();

			rectBox=new IsoBox();
			rectBox.fills=[null, null, null, null, null, new SolidColorFill(0xff0000, .5)];
			rectBox.setSize(tileSize, tileSize, 0);

			floorScene.addChild(rectBox);
			floorScene.render();

			view.addScene(floorScene);

			/** 场景 物体 **/
			mainScene=new IsoScene();
			//mainScene.layoutEnabled = true;
			mainScene.hostContainer=this;
			var factory:ClassFactory=new ClassFactory(DefaultShadowRenderer);
			factory.properties={shadowColor: 0x000000, shadowAlpha: 0.3, drawAll: true};
			mainScene.styleRenderers=[factory];
			
			
			view.addScene(mainScene);
			
			//前景
			frontScene = new IsoScene;
			var grid:IsoGrid=new IsoGrid();
			grid.showOrigin = false;
			grid.gridlines = new Stroke(0, 0xCCCCCC, 0.1);
			grid.cellSize=tileSize;
			grid.setGridSize(20, 20);
			grid.addEventListener(MouseEvent.CLICK, grid_mouseHandler);
			frontScene.addChild(grid);
			frontScene.render();
			view.addScene(frontScene);
			
			
			view.render();
		}
		
		//创建角色模型
		private function createSprite(username:String, x:int, y:int):IsoSprite
		{
			var spritex:IsoSprite=new IsoSprite();
			spritex.setSize(40, 40, 40);
			spritex.moveTo(x*tileSize, y*tileSize, 0);
			spritex.sprites=[new Character(username)];
			return spritex;
		}

		//鼠标事件
		private function grid_mouseHandler(evt:ProxyEvent):void
		{
			var mEvt:MouseEvent=MouseEvent(evt.targetEvent);
			var pt:Pt=new Pt(mEvt.localX, mEvt.localY);
			IsoMath.screenToIso(pt);

			var goal:IntPoint=screenToMap(pt.x, pt.y);
			if (evt.type == MouseEvent.CLICK)
			{
				if (tileMap.isWalkable(goal.x, goal.y))
				{
					var a:AStar=new AStar(tileMap, myinfo.nextpoint, goal);
					myinfo.solution = a.solve();
					myinfo.solution.pop();
					if (myinfo.solution)
					{
						this.rectBox.moveTo(goal.x * tileSize, goal.y * tileSize, 0);
						this.floorScene.render();

						if (myinfo.gt && myinfo.gt.position < 1 && myinfo.gt.paused==false)
						{
							//trace('myinfo timeStart return ');
							return;
						}
						timeStart(myinfo.gt, myinfo);
					}
				}
			}
		}

		private function pointToDirection(p1:IntPoint, p2:IntPoint):int
		{
			if (p1.x < p2.x)
			{
				return 0;
			}
			else if (p1.y < p2.y)
			{
				return 1;
			}
			else if (p1.x > p2.x)
			{
				return 2;
			}
			else if (p1.y > p2.y)
			{
				return 3;
			}
			return this.heroBox.sprites[0].getDirection();
		}

		private function screenToMap(x:int, y:int):IntPoint
		{
			return new IntPoint((x / this.tileSize)>>0, (y / this.tileSize)>>0);//Math.floor uint
		}

		//位置调整
		private function resizeHandler(e:Event):void
		{
			view.setSize(stage.stageWidth, stage.stageHeight);
			
			/** 小地图 **/
			minimap.x = stage.stageWidth - minimap.width;	

			/** 工具栏 **/
			toolContainer.y = stage.stageHeight - toolContainer.height;
			
			scrollPane.y = toolContainer.y - scrollPane.height;
		}

		//行走
		private function timeStart(e:GTween, user:User):void
		{
			if (user.solution.length > 0)
			{
				if (user.username == myinfo.username) 
				{
					var path:Array = user.solution.concat();
					path.push(user.nextpoint);
					minimap.drawPath(path);
					//minimap.drawPath(user.solution);
				}
				if (e==null)
					e=new GTween();
				
				e.target = user.display;
				e.paused = false;
				e.duration = 1;
				e.onComplete  = function(event:GTween):void{
					//trace('onComplete', user.username);
					user.point = user.nextpoint;
					timeStart(event, user);
				};
				e.onChange = function():void
				{
					if (user.username == myinfo.username)
					{
						view.centerOnIso(heroBox);
						view.render();
					}
					mainScene.render();
				};
				//trace(user.solution[user.solution.length-1].x, user.solution[user.solution.length-1].y);
				
				user.nextpoint = user.solution.pop();
				
				if (user.username==myinfo.username) sendMove(user.nextpoint.x, user.nextpoint.y);
				var p1:IntPoint=screenToMap(user.point.x, user.point.y);
				//trace('.toString()',user.point.toString(), user.nextpoint.toString());
				//trace(user.display.sprites[0]);
				if (user.display) user.display.sprites[0].setDirection(pointToDirection(user.point, user.nextpoint));
				
				if (user.display && user.display.sprites[0].state != CharacterStatic.walk)
				if (user.display) user.display.sprites[0].walk();
				e.setValues({"y":user.nextpoint.y * tileSize, "x":user.nextpoint.x * tileSize});
				
			}
			else
			{
				if (user.display) user.display.sprites[0].stand();
			}
			if (user.username==myinfo.username) minimap.drawRole(user.point);
			mainScene.render();
		}
	}
}
