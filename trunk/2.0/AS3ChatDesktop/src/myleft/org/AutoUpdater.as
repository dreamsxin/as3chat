package myleft.org {
	import flash.desktop.NativeApplication;
	import flash.desktop.Updater;
	import flash.display.DisplayObject;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.filesystem.File;
	import flash.filesystem.FileMode;
	import flash.filesystem.FileStream;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.system.Capabilities;
	
	import mx.controls.Alert;
	import mx.core.Application;
	import mx.core.FlexGlobals;
	import mx.events.CloseEvent;
	
	import org.myleft.data.Define;
	import org.myleft.events.AutoUpdaterEvent;
	
	[Event(name="versionCheckComplete", type="org.myleft.events.AutoUpdaterEvent")]

	public class AutoUpdater extends EventDispatcher{

		public var remoteVersion:String;
		
		private var _url:String;		
		private var versionLoader:URLLoader;
		private var airLoader:URLLoader;
		private var airUrl:String;
		
		public function AutoUpdater(url:String, target:IEventDispatcher=null)
		{
			this._url = url;
			super(target);
		}

		private function downloadAirFileCompleteHandler(event:Event):void {
			airLoader.removeEventListener(Event.COMPLETE, downloadAirFileCompleteHandler);
			airLoader.removeEventListener(IOErrorEvent.IO_ERROR, downloadAirFileIOErrorHandler);

			var airFile:File = File.applicationStorageDirectory.resolvePath("updateFile.air");
			var fs:FileStream = new FileStream();
			fs.open(airFile, FileMode.WRITE);
			fs.writeBytes(airLoader.data);
			fs.close();

			if (!Capabilities.isDebugger) {
				var updater:Updater = new Updater();
				updater.update(airFile, remoteVersion);
			}
		}

		private function downloadAirFileIOErrorHandler(event:IOErrorEvent):void {
			airLoader.removeEventListener(Event.COMPLETE, downloadAirFileCompleteHandler);
			airLoader.removeEventListener(IOErrorEvent.IO_ERROR, downloadAirFileIOErrorHandler);
			
			Alert.show('Air更新文件下载失败');
		}

		private function versionCheckCompleteHandler(event:Event):void {
			versionLoader.removeEventListener(Event.COMPLETE, versionCheckCompleteHandler);
			versionLoader.removeEventListener(IOErrorEvent.IO_ERROR, versionCheckIOErrorHandler);
			
			var xml:XML = new XML(versionLoader.data);
			remoteVersion = xml.version;
			airUrl = xml.url;
			
			var e:AutoUpdaterEvent = new AutoUpdaterEvent(AutoUpdaterEvent.VERSION_CHECK_COMPLETE);
			dispatchEvent( e );

		}

		private function versionCheckIOErrorHandler(event:IOErrorEvent):void {
			versionLoader.removeEventListener(Event.COMPLETE, versionCheckCompleteHandler);
			versionLoader.removeEventListener(IOErrorEvent.IO_ERROR, versionCheckIOErrorHandler);
			//Alert.show('检查更新失败');
		}
		
		public function checkUpdate():void {
			versionLoader = new URLLoader();
			versionLoader.addEventListener(Event.COMPLETE, versionCheckCompleteHandler);
			versionLoader.addEventListener(IOErrorEvent.IO_ERROR, versionCheckIOErrorHandler);
			versionLoader.load(new URLRequest(_url));
		}

		/////////////// Methods ////////////////
		public function downloadAirFile():void {
			airLoader = new URLLoader();
			airLoader.dataFormat = URLLoaderDataFormat.BINARY;
			airLoader.addEventListener(Event.COMPLETE, downloadAirFileCompleteHandler);
			airLoader.addEventListener(IOErrorEvent.IO_ERROR, downloadAirFileIOErrorHandler);
			airLoader.load(new URLRequest(airUrl));
		}
	}
}
