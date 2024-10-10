/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/collections/set.h"

#include "utils/common.h"
#include "utils/memory/memtrace.h"

/// Node in a binary search tree.
typedef struct set_node_t set_node_t;

struct set_node_t
{
  void* data;         ///< Pointer to the data stored in the node.
  set_node_t* parent; ///< Pointer to the parent node.
  set_node_t* left;   ///< Pointer to the left child node.
  set_node_t* right;  ///< Pointer to the right child node.
};

struct set_t
{
  set_cmp_func_t cmp; ///< Comparison function used to order elements in the set.
  size_t size;        ///< Number of elements in the set.
  set_node_t* root;   ///< Pointer to the root node of the binary search tree.
};

/**
 * \brief Initializes a new tree node.
 *
 * \param[in] data Pointer to the data to be associated with the node.
 * \returns Pointer to the newly initialized node.
 */
static set_node_t* set_node_init(void* data)
{
  set_node_t* node = (set_node_t*)malloc(sizeof(set_node_t));
  ASSERT(node != NULL);

  node->data = data;
  node->parent = NULL;
  node->left = NULL;
  node->right = NULL;

  return node;
}

/**
 * \brief Frees a tree node.
 *
 * \param[in] node Pointer to the node to be freed.
 */
static void set_node_free(set_node_t* node)
{
  free(node);
}

/**
 * \brief Frees the nodes of a set recursively.
 *
 * \param[in] node Pointer to the root node of the subtree to be freed.
 */
static void set_free_recur(set_node_t* node)
{
  if (node == NULL)
    return;

  set_free_recur(node->left);
  set_free_recur(node->right);

  free(node);
}

set_t* set_init(set_cmp_func_t cmp)
{
  set_t* set = (set_t*)malloc(sizeof(set_t));
  ASSERT(set != NULL);

  set->cmp = cmp;
  set->size = 0;
  set->root = NULL;

  return set;
}

void set_free(set_t* set)
{
  set_free_recur(set->root);
  free(set);
}

bool set_add(set_t* restrict set, void* restrict data)
{
  if (set->root == NULL)
  {
    set->root = set_node_init(data);
    set->size++;
    return true;
  }

  set_node_t* node = set->root;

  for (;;)
  {
    int rel = set->cmp(data, node->data);

    if (rel < 0)
    {
      if (node->left == NULL)
      {
        node->left = set_node_init(data);
        node->left->parent = node;
        set->size++;
        return true;
      }

      node = node->left;
    }
    else if (rel > 0)
    {
      if (node->right == NULL)
      {
        node->right = set_node_init(data);
        node->right->parent = node;
        set->size++;
        return true;
      }

      node = node->right;
    }
    else
      return false;
  }

  UNREACHABLE();

  return false;
}

bool set_remove(set_t* restrict set, void* restrict data)
{
  if (set->root == NULL)
    return false;

  set_node_t* node = set->root;

  int rel = set->cmp(data, node->data);

  for (;;)
  {
    if (rel < 0)
      node = node->left;
    else if (rel > 0)
      node = node->right;
    else
      break;

    if (node != NULL)
      rel = set->cmp(data, node->data);
    else
      break;
  }

  if (node == NULL)
    return false;

  if (node->left == NULL)
  {
    set_node_t* parent = node->parent;
    set_node_t* temp = node->right;

    if (parent == NULL)
    {
      if (temp != NULL)
        temp->parent = NULL;

      set->root = temp;
    }
    else if (node == parent->left)
      parent->left = temp;
    else
      parent->right = temp;

    set_node_free(node);
  }
  else if (node->right == NULL)
  {
    set_node_t* parent = node->parent;
    set_node_t* temp = node->left;

    if (parent == NULL)
    {
      if (temp != NULL)
        temp->parent = NULL;

      set->root = temp;
    }
    else if (node == parent->left)
    {
      if (temp != NULL)
        temp->parent = parent;

      parent->left = temp;
    }
    else
    {
      if (temp != NULL)
        temp->parent = parent;

      parent->right = temp;
    }

    set_node_free(node);
  }
  else
  {
    set_node_t* min_node = node->left;

    while (min_node->left != NULL)
      min_node = min_node->left;

    node->data = min_node->data;

    if (min_node->right != NULL)
      min_node->right->parent = min_node->parent;

    min_node->parent->left = min_node->right;

    set_node_free(min_node);
  }

  set->size--;

  return true;
}

void* set_get(const set_t* restrict set, const void* restrict data)
{
  set_node_t* node = set->root;

  while (node != NULL)
  {
    int rel = set->cmp(data, node->data);

    if (rel < 0)
      node = node->left;
    else if (rel > 0)
      node = node->right;
    else
      return node->data;
  }

  return NULL;
}

bool set_contains(const set_t* restrict set, const void* restrict data)
{
  set_node_t* node = set->root;

  while (node != NULL)
  {
    int rel = set->cmp(data, node->data);

    if (rel < 0)
      node = node->left;
    else if (rel > 0)
      node = node->right;
    else
      return true;
  }

  return false;
}

void* set_min(const set_t* set)
{
  if (set->root == NULL)
    return NULL;

  set_node_t* node = set->root;

  while (node->left != NULL)
    node = node->left;

  return node->data;
}

void* set_max(const set_t* set)
{
  if (set->root == NULL)
    return NULL;

  set_node_t* node = set->root;

  while (node->right != NULL)
    node = node->right;

  return node->data;
}

bool set_empty(const set_t* set)
{
  return set->size == 0;
}

size_t set_size(const set_t* set)
{
  return set->size;
}

void set_for_each(set_t* set, set_for_each_func_t func)
{
  set_node_t* node = set->root;
  set_node_t* prev = NULL;

  while (node != NULL)
  {
    if (prev == node->right && node->right != NULL)
    {
      prev = node;
      node = node->parent;
    }
    else
    {
      if (prev == node->parent)
        while (node->left != NULL)
          node = node->left;

      func(node->data);

      prev = node;
      node = node->right != NULL ? node->right : node->parent;
    }
  }
}
