#include "key_value_transation.h"

#if key_value_transation_compile_en

static key_value_register_t *register_head = NULL;
static key_value_register_t *register_tail = NULL;

static key_value_register_t *chek_has_register(key_value_handle_t handle);

#if key_value_support_rtos
typedef struct del_list_s
{
    key_value_handle_t handle;
    struct del_list_s *next;
} del_list_t;

static del_list_t *del_head = NULL;
static del_list_t *del_tail = NULL;
static key_value_mutex_cb_t mutex_cb;
void key_value_mutex_register(key_value_mutex_cb_t *cb)
{
    mutex_cb = *cb;
}
static uint8_t msg_busy = 0;
static uint8_t del_busy = 0;
#endif

static int key_cmp(const char *key1, const char *key2)
{
    for (; *key1 == *key2; ++key1, ++key2)
    {
        if (*key1 == '\0')
            return (0);
    }
    return 1;
}

static int get_key_sum(const char *key)
{
    int sum = 0, i;
    for (i = 0; i < strlen(key); i++)
    {
        sum += key[i];
    }
    return sum;
}

static key_value_register_t *chek_has_register(key_value_handle_t handle)
{
    key_value_handle_t move = (key_value_handle_t)register_head;

    if (register_head == NULL)
        return NULL;

    for (;;)
    {
        if (handle != move)
            goto _chek_next;

        return move;

    _chek_next:
        if (move->next == NULL)
            return NULL;
        move = move->next;
    }
}

/**
 * @brief 删除节点
 *
 * @param handle 句柄
 * @return int 0：成功，1：失败
 *  */
static int deL_handle(key_value_handle_t handle)
{
    key_value_register_t *tmp = chek_has_register(handle);
    if (tmp == NULL)
        return 1;

    if (tmp == register_head)
    {
        if (register_head == register_tail)
        {
            register_head = NULL;
            register_tail = NULL;
        }
        else
            register_head = register_head->next;
    }
    else if (tmp == register_tail)
    {
        register_tail = tmp->last;
        register_tail->next = NULL;
    }
    else
    {
        key_value_register_t *last = tmp->last;
        key_value_register_t *next = tmp->next;
        last->next = next;
        next->last = last;
    }

    key_value_free_func((void *)tmp->key);
    key_value_free_func(tmp);

    return 0;
}

#if key_value_support_rtos

static void chek_del_list()
{
    if (msg_busy)
        return;

    mutex_cb.mutex_get_cb();
    while (del_busy)
        ;

    if (del_head == NULL)
    {
        mutex_cb.mutex_give_cb();
        return;
    }

    for (;;)
    {
        del_list_t *tmp = del_head->next;
        deL_handle(del_head->handle);
        key_value_free_func(del_head);
        del_head = tmp;
        if (del_head == NULL)
        {
            del_tail = NULL;
            break;
        }
    }
    mutex_cb.mutex_give_cb();
}
#endif

int key_value_msg(const char *key, void *value, size_t lenth)
{
    int ret = 0;
    int sum = 0;

#if key_value_support_rtos

    if (msg_busy == 0)
    {
        mutex_cb.mutex_get_cb();
    }

    msg_busy++;
#endif

    key_value_register_t *move = register_head;

    if (move == NULL)
    {
        ret = 1;
        goto clear_out;
    }

    sum = get_key_sum(key);

    for (;;)
    {
        if (move->key_sum != sum)
            goto chek_null;

        if (key_cmp(key, move->key) == 0)
        {
            move->_cb(value, lenth);
        }

    chek_null:
        if (move->next == NULL)
            break;

        move = move->next;
    }

clear_out:
#if key_value_support_rtos
    msg_busy--;
    if (msg_busy == 0)
        mutex_cb.mutex_give_cb();

    chek_del_list();

#endif
    return ret;
}

int key_value_register(key_value_handle_t *handle, const char *key, key_value_cb_t cb)
{
    size_t _len = 0;
    key_value_register_t *tmp = key_value_malloc_func(sizeof(key_value_register_t));

    if (tmp == NULL)
        return 1;

    _len = strlen(key) + 1;
    tmp->key = key_value_malloc_func(_len);
    if (tmp->key == NULL)
    {
        key_value_free_func(tmp);
        return 2;
    }
    memcpy((void *)tmp->key, key, _len);

    tmp->key_sum = get_key_sum(key);
    tmp->next = NULL;
    tmp->last = NULL;
    tmp->_cb = cb;

#if key_value_support_rtos
    mutex_cb.mutex_get_cb();
#endif

    if (handle != NULL)
        *handle = tmp;

    if (register_head == NULL)
    {
        register_head = tmp;
        register_tail = tmp;
    }
    else
    {
        register_tail->next = tmp;
        tmp->last = register_tail;
        register_tail = tmp;
    }

#if key_value_support_rtos
    mutex_cb.mutex_give_cb();
#endif

    return 0;
}

int key_value_del(key_value_handle_t handle)
{

#if key_value_support_rtos
    if (msg_busy)
    {
        del_list_t *tmp = key_value_malloc_func(sizeof(del_list_t));
        if (tmp == NULL)
            return 2;
        del_busy = 1;
        tmp->next = NULL;
        tmp->handle = handle;

        if (del_tail != NULL)
            del_tail->next = tmp;
        del_tail = tmp;
        if (del_head == NULL)
            del_head = tmp;
        del_busy = 0;
        return 0;
    }

    mutex_cb.mutex_get_cb();
#endif

    int ret = deL_handle(handle);

#if key_value_support_rtos
    mutex_cb.mutex_give_cb();
#endif

    return ret;
}

#endif
