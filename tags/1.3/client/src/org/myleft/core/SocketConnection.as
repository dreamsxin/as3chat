package org.myleft.core
{
	import flash.system.Security;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.events.SecurityErrorEvent;
	import flash.events.TimerEvent;
	import flash.net.Socket;
	import flash.utils.ByteArray;
	import flash.utils.Timer;
	import flash.xml.XMLDocument;
	import flash.xml.XMLNode;
	
	import org.myleft.data.Define;
	import org.myleft.events.SocketConnectionEvent;
	
	public class SocketConnection extends EventDispatcher
	{
		public var socket:Socket;
		public var host:String;
		public var port:int;
		
		public var keepAlive:Timer;
		private var lastSendTime:Number;


		
		public function SocketConnection(target:IEventDispatcher=null)
		{
			super(target);
			socket = new Socket;
			
			socket.addEventListener(Event.CONNECT, socketEventFunction);
			socket.addEventListener(Event.CLOSE, socketEventFunction);
			
			socket.addEventListener(Event.ACTIVATE, socketEventFunction);
			socket.addEventListener(IOErrorEvent.IO_ERROR, socketIOErrorEventFunction);
			socket.addEventListener(SecurityErrorEvent.SECURITY_ERROR, socketSecurityErrorEventFunction);
			
			socket.addEventListener(ProgressEvent.SOCKET_DATA, socketDataFunction);
			
			keepAlive = new Timer(100000);
  			keepAlive.addEventListener(TimerEvent.TIMER, onKeepAliveLoop);

		}
		
		private function onKeepAliveLoop(evt:TimerEvent):void
		{       
			
			//if((new Date().getTime() - lastSendTime > 15000))
			if ( socket.connected ) {			
				this.sendKeepAlive();
			}
			else
			{
				keepAlive.stop();
				
				var e:SocketConnectionEvent;
				e = new SocketConnectionEvent(Define.CLOSE);
				dispatchEvent( e );
			}
			
		}

		
		public function connect(host:String, port:int):void
		{
			Security.loadPolicyFile("xmlsocket://"+host+":"+port.toString());
			this.host = host;
			this.port = port;
			socket.connect(this.host, this.port);
		}

		public function disconnect():void
		{
			keepAlive.stop();
			socket.close();
		}

		public function send( message:String ):void
		{
			trace('send', message);
			socket.writeUTFBytes(message);
			socket.flush();
		}

		public function sendKeepAlive():void
		{
			this.send("<ping/>");
		}
		
		private function socketEventFunction(event:Event):void
		{
			trace("socketEventFunction", event.type)
			var e:SocketConnectionEvent;
			switch (event.type)
			{
				case Event.CONNECT:
					//this.send(Event.CONNECT);
					e = new SocketConnectionEvent(Define.CONNECT);
					keepAlive.start();
					break;
				case Event.CLOSE:
					//this.send(Event.CONNECT);
					e = new SocketConnectionEvent(Define.CLOSE);
					keepAlive.stop();
					break;
				case Event.ACTIVATE:
					e = new SocketConnectionEvent(Define.ACTIVATE);
					break;
				default:
					e = new SocketConnectionEvent(Define.NODEFINE);
			}
			
			//dispatchEvent( new Event(Event.CONNECT) );
			dispatchEvent( e );
		}
		
		private function socketIOErrorEventFunction(event:IOErrorEvent):void
		{
			var e:SocketConnectionEvent;
			e = new SocketConnectionEvent(Define.CLOSE);
			dispatchEvent( e );
		}
		
		
		private function socketSecurityErrorEventFunction(event:SecurityErrorEvent):void
		{
			var e:SocketConnectionEvent;
			e = new SocketConnectionEvent(Define.CLOSE);
			dispatchEvent( e );
		}
		
		private function socketDataFunction(e:ProgressEvent):void
		{
			var bytedata:ByteArray = new ByteArray();
			socket.readBytes( bytedata );
			parseDataReceived(bytedata);
		}
		
		private function parseDataReceived( bytedata:ByteArray ):void
		{
			bytedata.position = 0;
			
			var data:String = bytedata.readUTFBytes( bytedata.length );
			trace('parseData:', data);
			var xmlData:XMLDocument = new XMLDocument();
			xmlData.ignoreWhite = true;
			
			var xml:XMLList;
			try
			{
				//xml = new XML(rawXML);

				var isComplete: Boolean = true;
				xmlData.parseXML( data );
				var len:uint = xmlData.childNodes.length;
				for (var i:int = 0; i < len; ++i)
				{
					// Read the data and send it to the appropriate parser
					var currentNode:XMLNode = xmlData.childNodes[i];
					trace('currentNode', currentNode.toString());
					handleNodeType(currentNode);
				}
		
			}
			catch (err:Error)
			{
				trace(err.toString());
			}
		}
		
		private function handleNodeType(node:XMLNode):void
		{
			var nodeName:String = node.nodeName.toLowerCase();
			
			var e:SocketConnectionEvent;
			trace('nodeName:', nodeName);
			e = new SocketConnectionEvent(node.attributes.type);
			e.body = node;
			dispatchEvent( e );
		}
	}
}