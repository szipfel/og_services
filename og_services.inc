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
      $result['status'] = "400";
      $result['message'] = "Could not find the user with uuid " . $data['user_uuid'];
    }
    if(!$nid = db_query("SELECT nid FROM {node} WHERE uuid = :uuid", array(':uuid' => $data['node_uuid']))->fetchField()) {
      $result['status'] = "400";
      $result['message'] = "Could not find the node with uuid " . $data['node_uuid'];
    }
    // make an entry into the groups for this user and this node.
    if(isset($uid) && isset($nid)) {
      if (_og_group_add($uid, $nid)) {
        $result['result'] = 'true';
      }
    } else {
      $result['status'] = "400";
      $result['message'] = "Missing Data";
    }

  } else {
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
  if($uid > 1 && $nid > 0) {
    return _og_group_remove($uid, $nid);
  } else if ($uid > 0 && !$nid) {
    return _og_group_remove_all($uid);
  } else {
    $result['status'] = "400";
    $result['message'] = "Missing Data";
    return $result;
  }

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

function _og_group_remove_all($uid) {
$result = array();
  $delete = db_delete('og_membership')
    ->condition('etid', $uid)
    ->condition('entity_type', 'user')
    ->execute();

    $result['status'] = "200";
    $result['message'] = $delete . " memberships revoked";
    return $result;

}

function _og_services_retrieve($data){
  global $language;
  $result = array();
  $items = explode(':', $data);

  // If node ID is not set then return all available reports for the provided user.
  if (isset($items[1]) && isset($items[0])) {
    $uid = db_query("SELECT uid FROM {users} WHERE uuid = :uuid", array(':uuid' => $items[0]))->fetchField();
    $nid = db_query("SELECT nid FROM {node} WHERE uuid = :uuid", array(':uuid' => $items[1]))->fetchField();
    if (($uid != NULL) && ($nid != NULL)) {
      if (og_is_member('node', $nid, 'user', $uid)) { // are they actually a member?
        $result['status'] = "200";
        $result['message'] = "true";
      }
      else {
        $result['status'] = "200";
        $result['message'] = "false";
      }
    }
    else {
      $result['status'] = "400";
      $result['message'] = "Missing Data";
    }
  } else if (!isset($items[1]) && isset($items[0])) {
    $og_memberships = array();
    $uid = db_query("SELECT uid FROM {users} WHERE uuid = :uuid", array(':uuid' => $items[0]))->fetchField();
    $user = user_load($uid);
    foreach($user->og_user_node[LANGUAGE_NONE] as $key => $value) {
      $node = node_load(array_shift(array_values($value)));
      // send a list of UUIDs that represent the groups the user belongs to.
      $og_memberships[] = $node->uuid;
    }

    $result['status'] = "200";
    $result['message'] = $og_memberships;
  } else {
    $result['status'] = "400";
    $result['message'] = "Missing Data";
  }
   return $result;
}

?>
