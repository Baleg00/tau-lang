/**
 * \file
 * 
 * \copyright Copyright (c) 2023 Róna Balázs. All rights reserved.
 * \license This project is released under the Apache 2.0 license.
 */

#include "utils/collections/set.h"

/// Node in a binary search tree.
typedef struct tau_set_node_t tau_set_node_t;

struct tau_set_node_t
{
  void* data;         ///< Pointer to the data stored in the node.
  tau_set_node_t* parent; ///< Pointer to the parent node.
  tau_set_node_t* left;   ///< Pointer to the left child node.
  tau_set_node_t* right;  ///< Pointer to the right child node.
};

struct tau_set_t
{
  tau_set_cmp_func_t cmp; ///< Comparison function used to order elements in the set.
  size_t size;        ///< Number of elements in the set.
  tau_set_node_t* root;   ///< Pointer to the root node of the binary search tree.
};

/**
 * \brief Initializes a new tree node.
 *
 * \param[in] data Pointer to the data to be associated with the node.
 * \returns Pointer to the newly initialized node.
 */
static tau_set_node_t* tau_set_node_init(void* data)
{
  tau_set_node_t* node = (tau_set_node_t*)malloc(sizeof(tau_set_node_t));
  TAU_ASSERT(node != NULL);

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
static void tau_set_node_free(tau_set_node_t* node)
{
  free(node);
}

/**
 * \brief Frees the nodes of a set recursively.
 *
 * \param[in] node Pointer to the root node of the subtree to be freed.
 */
static void tau_set_free_recur(tau_set_node_t* node)
{
  if (node == NULL)
    return;

  tau_set_free_recur(node->left);
  tau_set_free_recur(node->right);

  free(node);
}

tau_set_t* tau_set_init(tau_set_cmp_func_t cmp)
{
  tau_set_t* set = (tau_set_t*)malloc(sizeof(tau_set_t));
  TAU_ASSERT(set != NULL);

  set->cmp = cmp;
  set->size = 0;
  set->root = NULL;

  return set;
}

void tau_set_free(tau_set_t* set)
{
  tau_set_free_recur(set->root);
  free(set);
}

bool tau_set_add(tau_set_t* restrict set, void* restrict data)
{
  if (set->root == NULL)
  {
    set->root = tau_set_node_init(data);
    set->size++;
    return true;
  }

  tau_set_node_t* node = set->root;

  for (;;)
  {
    int rel = set->cmp(data, node->data);

    if (rel < 0)
    {
      if (node->left == NULL)
      {
        node->left = tau_set_node_init(data);
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
        node->right = tau_set_node_init(data);
        node->right->parent = node;
        set->size++;
        return true;
      }

      node = node->right;
    }
    else
      return false;
  }

  TAU_UNREACHABLE();

  return false;
}

bool tau_set_remove(tau_set_t* restrict set, void* restrict data)
{
  if (set->root == NULL)
    return false;

  tau_set_node_t* node = set->root;

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
    tau_set_node_t* parent = node->parent;
    tau_set_node_t* temp = node->right;

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

    tau_set_node_free(node);
  }
  else if (node->right == NULL)
  {
    tau_set_node_t* parent = node->parent;
    tau_set_node_t* temp = node->left;

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

    tau_set_node_free(node);
  }
  else
  {
    tau_set_node_t* min_node = node->left;

    while (min_node->left != NULL)
      min_node = min_node->left;

    node->data = min_node->data;

    if (min_node->right != NULL)
      min_node->right->parent = min_node->parent;

    min_node->parent->left = min_node->right;

    tau_set_node_free(min_node);
  }

  set->size--;

  return true;
}

void* tau_set_get(const tau_set_t* restrict set, const void* restrict data)
{
  tau_set_node_t* node = set->root;

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

bool tau_set_contains(const tau_set_t* restrict set, const void* restrict data)
{
  tau_set_node_t* node = set->root;

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

void* tau_set_min(const tau_set_t* set)
{
  if (set->root == NULL)
    return NULL;

  tau_set_node_t* node = set->root;

  while (node->left != NULL)
    node = node->left;

  return node->data;
}

void* tau_set_max(const tau_set_t* set)
{
  if (set->root == NULL)
    return NULL;

  tau_set_node_t* node = set->root;

  while (node->right != NULL)
    node = node->right;

  return node->data;
}

bool tau_set_empty(const tau_set_t* set)
{
  return set->size == 0;
}

size_t tau_set_size(const tau_set_t* set)
{
  return set->size;
}

void tau_set_for_each(tau_set_t* set, tau_set_for_each_func_t func)
{
  tau_set_node_t* node = set->root;
  tau_set_node_t* prev = NULL;

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
