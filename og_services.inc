<?php

/**
 * @param $data
 */
function _og_services_create($data) {
print_r($data);
  //find the matching node id based on the UUID:
  $uid = db_query("SELECT uid FROM {users} WHERE uuid = :uuid", array(':uuid' => $data['user_uuid']))->fetchField();
  $nid = db_query("SELECT nid FROM {node} WHERE uuid = :uuid", array(':uuid' => $data['node_uuid']))->fetchField();

  // make an entry into the groups for this user and this node.
  _og_group_add($uid, $nid);

}

/**
 * @param $data
 * @return bool
 */
function _og_services_delete($data) {
print_r($data);
  print "======= sd=fsdf sdf=======";
  //$uid = db_query("SELECT uid FROM {users} WHERE uuid = :uuid", array(':uuid' => $data['user_uuid']))->fetchField();

 //if(_og_group_remove($uid, $gid)) {
   return TRUE;
 //}
}

/**
 * @param $uid
 * @param $gid
 */
function _og_group_add($uid, $gid) {

    $user = user_load($uid);

    og_group('node', $gid, array(
      "entity type" => "user",
      "entity" => $user,
      "membership type" => OG_MEMBERSHIP_TYPE_DEFAULT,
    ));
    // Changes the users role in the group (1 = non-member, 2 = member, 3 = administrator member)
    og_role_grant('node', $gid, $user->uid, 2);

}

/**
 * Remove the organic group membership for user x from group id: y
 *
 * @param $uid The User ID of the user to revoke group membership from
 * @param $gid The Node ID of the group that we are removing the user from .
 * @return bool
 */
/*
function _og_group_remove($uid, $gid) {

  if(og_ungroup('node', $gid, $entity_type = 'user', $uid)) {
    return TRUE;
  } else {
    return FALSE;
  }

}
*/
?>