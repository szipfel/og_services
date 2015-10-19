<?php

function _og_services_create($data) {
 // $user = user_load($data['uid']);

  //find the matching node id based on the UUID:
  $nid = db_query("SELECT nid FROM {node} WHERE uuid = :uuid", array(':uuid' => $data['uuid']))->fetchField();

  // make an entry into the groups for this user and this node.
  _group_add($data['uid'], $nid);

}


function _group_add($uid, $gid) {

    $user = user_load($uid);

    og_group('node', $gid, array(
      "entity type" => "user",
      "entity" => $user,
      "membership type" => OG_MEMBERSHIP_TYPE_DEFAULT,
    ));
    // Changes the users role in the group (1 = non-member, 2 = member, 3 = administrator member)
    og_role_grant('node', $gid, $user->uid, 2);

}

?>