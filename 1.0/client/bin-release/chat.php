<?php
/*
	[UCenter Home] (C) 2007-2008 Comsenz Inc.
	$Id: userapp.php 13003 2009-08-05 06:46:06Z liguode $
*/

include_once('./common.php');

//是否关闭站点
checkclose();

$base = 'myleftchat/';
$url = $base.'myleftchatclient.swf';
$username = '游客'.mt_rand(10, 99).date('dHis');

if(!empty($_SGLOBAL['supe_uid']))
{

	//空间信息
	$space = getspace($_SGLOBAL['supe_uid']);

	//空间被锁定
	if($space['flag'] == -1) {
		showmessage('space_has_been_locked');
	}

	if(checkperm('banvisit')) {
		ckspacelog();
		showmessage('you_do_not_have_permission_to_visit');
	}
	if(empty($_SCONFIG['my_status'])) {
		showmessage('no_privilege_my_status');
	}
	$username = $space['username'];

}
$timestamp = time();
$sign = crypt('myleft200011'.$username.$timestamp);

$url = $url.'?autologin=1&account=myleft&username='.$username.'&timestamp='.$timestamp.'&sign='.$sign;
include_once template("chat");

?>
