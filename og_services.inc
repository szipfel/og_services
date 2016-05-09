<?php

/**
 * @param $data
 */
function _og_services_create($data) {
  $result = array();
  $result['result'] = 'false';

  if(isset($data['user_uuid']) && isset($data['node_uuid'])) {
    //find the matching node id based on the UUID:
    if(!$uid = db_query("SELECT uid FROM {users} WHERE uuid = :uuid", array(':uuid' => $data['user_uuid']))->fetchField()) {
      http_response_code(400);
      $result['status'] = "400";
      $result['message'] = "Could not find the user with uuid " . $data['user_uuid'];
    }
    if(!$nid = db_query("SELECT nid FROM {node} WHERE uuid = :uuid", array(':uuid' => $data['node_uuid']))->fetchField()) {
      http_response_code(400);
      $result['status'] = "400";
      $result['message'] = "Could not find the node with uuid " . $data['node_uuid'];
    }
    // make an entry into the groups for this user and this node.
    if(isset($uid) && isset($nid)) {
      if (_og_group_add($uid, $nid)) {
        $result['result'] = 'true';
      }
    } else {
      http_response_code(400);
      $result['status'] = "400";
      $result['message'] = "Missing Data";
    }

  } else {
    http_response_code(400);
    $result['status'] = "400";
    $result['message'] = "Missing Data";
  }

  return $result;
}

/**
 * @param $data
 * @return bool
 */
function _og_services_delete($data) {

  $items = explode(':', $data);

  $uid = db_query("SELECT uid FROM {users} WHERE uuid = :uuid", array(':uuid' => $items[0]))->fetchField();
  $nid= db_query("SELECT nid FROM {node} WHERE uuid = :uuid", array(':uuid' => $items[1]))->fetchField();

  _og_group_remove($uid, $nid);

}

/**
 * @param $uid
 * @param $gid
 */
function _og_group_add($uid, $gid) {

  $user = user_load($uid);

  if(og_group('node', $gid, array(
    "entity type" => "user",
    "entity" => $user,
    "membership type" => OG_MEMBERSHIP_TYPE_DEFAULT,
  ))) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 * Remove the organic group membership for user x from group id: y
 *
 * @param $uid The User ID of the user to revoke group membership from
 * @param $gid The Node ID of the group that we are removing the user from .
 * @return bool
 */

function _og_group_remove($uid, $gid) {

  if(og_ungroup('node', $gid, $entity_type = 'user', $uid)) {
    return TRUE;
  } else {
    return FALSE;
  }

}

function _og_services_retrieve($data){
  $result = array();
  $items = explode(':', $data);

  $uid = db_query("SELECT uid FROM {users} WHERE uuid = :uuid", array(':uuid' => $items[0]))->fetchField();
  $nid = db_query("SELECT nid FROM {node} WHERE uuid = :uuid", array(':uuid' => $items[1]))->fetchField();
  if(($uid != NULL) && ($nid != NULL)) {
    if (og_is_member('node', $nid, 'user', $uid)) { // are they actually a member?
      http_response_code(200);
      $result['status'] = "200";
      $result['message'] = "true";
      return $result;
    } else {
      http_response_code(200);
      $result['status'] = "200";
      $result['message'] = "false";
      return $result;
    }
  } else {
    http_response_code(400);
    $result['status'] = "400";
    $result['message'] = "Missing Data";
    return $result;
  }
}

?>
