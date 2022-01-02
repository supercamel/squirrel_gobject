#ifndef LIBSQUIRREL3_VM_INCLUDED
#define LIBSQUIRREL3_VM_INCLUDED

#include <glib-object.h>
#include <squirrel.h>

G_BEGIN_DECLS

#define SQUIRREL_TYPE_VM (squirrel_vm_get_type())

G_DECLARE_FINAL_TYPE (SquirrelVm, squirrel_vm, Squirrel, Vm, GObject)


typedef enum {
    SQUIRREL_OBJECTTYPE_NULL = OT_NULL,
    SQUIRREL_OBJECTTYPE_INTEGER = OT_INTEGER,
    SQUIRREL_OBJECTTYPE_FLOAT = OT_FLOAT,
    SQUIRREL_OBJECTTYPE_BOOL = OT_BOOL,
    SQUIRREL_OBJECTTYPE_STRING = OT_STRING,
    SQUIRREL_OBJECTTYPE_TABLE = OT_TABLE,
    SQUIRREL_OBJECTTYPE_ARRAY = OT_ARRAY,
    SQUIRREL_OBJECTTYPE_USERDATA = OT_USERDATA,
    SQUIRREL_OBJECTTYPE_CLOSURE = OT_CLOSURE,
    SQUIRREL_OBJECTTYPE_NATIVECLOSURE = OT_NATIVECLOSURE,
    SQUIRREL_OBJECTTYPE_GENERATOR = OT_GENERATOR,
    SQUIRREL_OBJECTTYPE_USERPOINTER = OT_USERPOINTER,
    SQUIRREL_OBJECTTYPE_THREAD = OT_THREAD,
    SQUIRREL_OBJECTTYPE_FUNCPROTO = OT_FUNCPROTO,
    SQUIRREL_OBJECTTYPE_CLASS = OT_CLASS,
    SQUIRREL_OBJECTTYPE_INSTANCE = OT_INSTANCE,
    SQUIRREL_OBJECTTYPE_WEAKREF = OT_WEAKREF,
    SQUIRREL_OBJECTTYPE_OUTER = OT_OUTER
} SquirrelOBJECTTYPE;

#define SQUIRREL_TYPE_OBJECTTYPE (squirrel_objecttype_get_type())

#define SQUIRREL_TYPE_OBJ (squirrel_obj_get_type())
typedef struct tagSQObject SquirrelObj;


#define SQUIRREL_TYPE_FUNCTION_INFO (squirrel_function_info_get_type())

typedef struct {
    gpointer funcid;
    const gchar* name;
    const gchar* source;
    glong line;
} SquirrelFunctionInfo;

#define SQUIRREL_TYPE_MEMBER_HANDLE (squirrel_member_handle_get_type())

typedef struct {
    gboolean _static;
    glong _index;
} SquirrelMemberHandle;


typedef glong (*SquirrelClosure)(gpointer hvm, gpointer user_data);

typedef glong (*SquirrelReleaseHook) (
        gpointer user_pointer, 
        glong size, 
        gpointer user_data
        );

typedef glong (*SquirrelWriteFunc)(
        gpointer ptr, 
        gpointer user_pointer, 
        glong i, 
        gpointer user_data);

typedef glong (*SquirrelReadFunc)(
        gpointer ptr, 
        gpointer user_pointer, 
        glong i, 
        gpointer user_data);


/**
 * squirrel_vm_from_hvm:
 * @ptr: the VM pointer
 * Return: (transfer none): 
 */
SquirrelVm* squirrel_vm_from_hvm(gpointer ptr);

/**
 * squirrel_vm_new:
 * @initial_stack_size: initial stack size for the VM, 1024 is usually a good number
 * Return: (transfer full): 
 */
SquirrelVm* squirrel_vm_new(glong initial_stack_size);
gboolean squirrel_vm_do_file(SquirrelVm* self, const gchar* path, gboolean retval, gboolean print_error);


/**
 * squirrel_vm_test:
 * @self: the self
 * @c: (scope call): the callback
 */
void squirrel_vm_test(SquirrelVm* self, SquirrelClosure c);

glong squirrel_vm_suspend(SquirrelVm* self);
glong squirrel_vm_wake_up(SquirrelVm* self, 
        gboolean resumedret, 
        gboolean retval, 
        gboolean raiseerror, 
        gboolean throwerror);

glong squirrel_vm_get_state(SquirrelVm* self);
glong squirrel_vm_get_version(SquirrelVm* self);

//glong sq_compile(SquirrelVm* self, 

// stack operations
void squirrel_vm_push(SquirrelVm* self, glong idx);
void squirrel_vm_pop(SquirrelVm* self, gulong sz);
void squirrel_vm_pop_top(SquirrelVm* self);
void squirrel_vm_remove(SquirrelVm* self, glong idx);
glong squirrel_vm_get_top(SquirrelVm* self);
void squirrel_vm_set_top(SquirrelVm* self, glong newtop);
glong squirrel_vm_reserve_stack(SquirrelVm* self, glong nsize);
glong squirrel_vm_cmp(SquirrelVm* self);
void squirrel_vm_move(SquirrelVm* self, SquirrelVm* dest, glong idx);


// object creation handling
void squirrel_vm_new_table(SquirrelVm* self);
void squirrel_vm_new_table_ex(SquirrelVm* self, glong initial_capacity);
void squirrel_vm_new_array(SquirrelVm* self, glong size);

/**
 * squirrel_vm_new_closure:
 * @self: the self
 * @c: (scope notified): the callback
 * @nfreevars: 
 */
void squirrel_vm_new_closure(SquirrelVm* self, SquirrelClosure c, gulong nfreevars);
glong squirrel_vm_set_params_check(SquirrelVm* self, glong nparamscheck, const gchar* typemark);
glong squirrel_vm_bind_env(SquirrelVm* self, glong idx);
glong squirrel_vm_set_closure_root(SquirrelVm* self, glong idx);
glong squirrel_vm_get_closure_root(SquirrelVm* self, glong idx);
void squirrel_vm_push_string(SquirrelVm* self, const gchar* s);
void squirrel_vm_push_int(SquirrelVm* self, glong n);
void squirrel_vm_push_bool(SquirrelVm* self, gboolean b);
void squirrel_vm_push_user_pointer(SquirrelVm* self, gpointer p);
void squirrel_vm_push_null(SquirrelVm* self);
void squirrel_vm_push_thread(SquirrelVm* self, SquirrelVm* thread);
SquirrelOBJECTTYPE squirrel_vm_get_object_type(SquirrelVm* self, glong idx);
glong squirrel_vm_get_typeof(SquirrelVm* self, glong idx);
glong squirrel_vm_get_size(SquirrelVm* self, glong idx);
glong squirrel_vm_get_hash(SquirrelVm* self, glong idx);
glong squirrel_vm_get_base(SquirrelVm* self, glong idx);
gboolean squirrel_vm_instance_of(SquirrelVm* self);
glong squirrel_vm_to_string(SquirrelVm* self, glong idx);

/**
 * squirrel_vm_to_bool:
 * @self: the self
 * @idx: index
 * @b: (out): the boolean
 */
void squirrel_vm_to_bool(SquirrelVm* self, glong idx, gboolean* b);

/**
 * squirrel_vm_get_string:
 * @self: the self
 * @idx: index
 * @s: (out)(transfer full): the string
 */
glong squirrel_vm_get_string(SquirrelVm* self, glong idx, const gchar** s);

/**
 * squirrel_vm_get_int:
 * @self: the self
 * @idx: index
 * @i: (out): the int
 */
glong squirrel_vm_get_int(SquirrelVm* self, glong idx, glong* i);

/**
 * squirrel_vm_get_float:
 * @self: the self
 * @idx: index
 * @f: (out): the float
 */
glong squirrel_vm_get_float(SquirrelVm* self, glong idx, gfloat* f);

/**
 * squirrel_vm_get_bool:
 * @self: the self
 * @idx: index
 * @b: (out): the bool 
 */
glong squirrel_vm_get_bool(SquirrelVm* self, glong idx, gboolean* b);

/**
 * squirrel_vm_get_thread:
 * @self: the self
 * @idx: index
 * @thread: (out)(transfer full): the thread
 */
glong squirrel_vm_get_thread(SquirrelVm* self, glong idx, SquirrelVm* thread);


/**
 * squirrel_vm_get_user_pointer:
 * @self: the self
 * @idx: index
 * @ptr: (out)(transfer none): the pointer
 */
glong squirrel_vm_get_user_pointer(SquirrelVm* self, glong idx, gpointer* ptr);

/**
 * squirrel_vm_get_user_data:
 * @self: the self
 * @idx: index
 * @ptr: (out)(transfer none): the pointer
 * @typetag: (out)(transfer none): the type tag
 */
glong squirrel_vm_get_user_data(
        SquirrelVm* self, 
        glong idx, 
        gpointer* ptr, 
        gpointer* typetag);

/**
 * squirrel_vm_set_type_tag:
 */
glong squirrel_vm_set_type_tag(SquirrelVm* self, glong idx, gpointer typetag);

/**
 * squirrel_vm_get_type_tag:
 * @self: the self
 * @idx: index
 * @typetag: (out)(transfer none): the type tag
 */
glong squirrel_vm_get_type_tag(SquirrelVm* self, glong idx, gpointer* typetag);

/**
 * squirrel_vm_set_release_hook:
 * @self: the self
 * @idx: the index
 * @hook: (scope notified): the hook
 */
void squirrel_vm_set_release_hook(SquirrelVm* self, glong idx, SquirrelReleaseHook hook);

/**
 * squirrel_vm_get_release_hook: 
 * (skip)
 */
SquirrelReleaseHook squirrel_vm_get_release_hook(SquirrelVm* self, glong idx);

gchar* squirrel_vm_get_scratch_pad(SquirrelVm* self, glong idx);

/**
 * squirrel_vm_get_function_info:
 * @self: the self
 * @level: the level
 * @fi: (out)(transfer full): fi
 */
glong squirrel_vm_get_function_info(SquirrelVm* self, glong level, SquirrelFunctionInfo* fi);

/**
 * squirrel_vm_get_closure_info:
 * @self: the self
 * @idx: the idx
 * @nparams: (out):
 * @nfreevars: (out):
 */
glong squirrel_vm_get_closure_info(SquirrelVm* self, glong idx, gulong* nparams, gulong* nfreevars);

glong squirrel_vm_get_closure_name(SquirrelVm* self, glong idx);
glong squirrel_vm_set_native_closure_name(SquirrelVm* self, glong idx, const gchar* name);
glong squirrel_vm_set_instance_up(SquirrelVm* self, glong idx, gpointer p);
glong squirrel_vm_get_instance_up(SquirrelVm* self, glong idx, gpointer* p, gpointer typetag);
glong squirrel_vm_set_class_ud_size(SquirrelVm* self, glong idx, glong udsize);
glong squirrel_vm_new_class(SquirrelVm* self, gboolean hasbase);
glong squirrel_vm_create_instance(SquirrelVm* self, glong idx);
glong squirrel_vm_set_attributes(SquirrelVm* self, glong idx);
glong squirrel_vm_get_attributes(SquirrelVm* self, glong idx);
glong squirrel_vm_get_class(SquirrelVm* self, glong idx);
void squirrel_vm_weak_ref(SquirrelVm* self, glong idx);
glong squirrel_vm_get_default_delegate(SquirrelVm* self, SquirrelOBJECTTYPE Ot);
glong squirrel_vm_get_member_handle(
        SquirrelVm* self, glong idx, SquirrelMemberHandle* handle);
glong squirrel_vm_get_by_handle(
        SquirrelVm* self, glong idx, const SquirrelMemberHandle* handle);
glong squirrel_vm_set_by_handle(
        SquirrelVm* self, glong idx, const SquirrelMemberHandle* handle);



// object manipulation
void squirrel_vm_push_root_table(SquirrelVm* self);
void squirrel_vm_push_registry_table(SquirrelVm* self);
void squirrel_vm_push_const_table(SquirrelVm* self);
glong squirrel_vm_set_root_table(SquirrelVm* self);
glong squirrel_vm_set_const_table(SquirrelVm* self);
glong squirrel_vm_new_slot(SquirrelVm* self, glong idx, gboolean bstatic);
glong squirrel_vm_delete_slot(SquirrelVm* self, glong idx, gboolean pushval);
glong squirrel_vm_set(SquirrelVm* self, glong idx);
glong squirrel_vm_get(SquirrelVm* self, glong idx);
glong squirrel_vm_raw_set(SquirrelVm* self, glong idx);
glong squirrel_vm_raw_get(SquirrelVm* self, glong idx);
glong squirrel_vm_raw_delete_slot(SquirrelVm* self, glong idx, gboolean pushval);
glong squirrel_vm_new_member(SquirrelVm* self, glong idx, gboolean bstatic);
glong squirrel_vm_raw_new_member(SquirrelVm* self, glong idx, gboolean bstatic);
glong squirrel_vm_array_append(SquirrelVm* self, glong idx);
glong squirrel_vm_array_pop(SquirrelVm* self, glong idx, gboolean pushval);
glong squirrel_vm_array_resize(SquirrelVm* self, glong idx, glong newsize);
glong squirrel_vm_array_reverse(SquirrelVm* self, glong idx);
glong squirrel_vm_array_remove(SquirrelVm* self, glong idx, glong itemidx);
glong squirrel_vm_array_insert(SquirrelVm* self, glong idx, glong destpos);
glong squirrel_vm_set_delegate(SquirrelVm* self, glong idx);
glong squirrel_vm_get_delegate(SquirrelVm* self, glong idx);
glong squirrel_vm_clone(SquirrelVm* self, glong idx);
glong squirrel_vm_set_free_variable(SquirrelVm* self, glong idx, gulong nval);
glong squirrel_vm_next(SquirrelVm* self, glong idx);
glong squirrel_vm_get_weak_ref_val(SquirrelVm* self, glong idx);
glong squirrel_vm_clear(SquirrelVm* self, glong idx);

// calls
glong squirrel_vm_call(SquirrelVm* self, glong params, gboolean retval, gboolean raiseerror);
glong squirrel_vm_resume(SquirrelVm* self, gboolean retval, gboolean raiseerror);
gchar* squirrel_vm_get_local(SquirrelVm* self, gulong level, gulong idx);
glong squirrel_vm_get_callee(SquirrelVm* self);
gchar* squirrel_vm_get_free_variable(SquirrelVm* self, glong idx, gulong nval);
glong squirrel_vm_throw_error(SquirrelVm* self, const gchar* err);
glong squirrel_vm_throw_object(SquirrelVm* self);
void squirrel_vm_reset_error(SquirrelVm* self);
void squirrel_vm_get_last_error(SquirrelVm* self);


// raw object handling
glong squirrel_vm_get_stack_obj(SquirrelVm* self, glong idx, SquirrelObj* po);
void squirrel_vm_push_object(SquirrelVm* self, SquirrelObj obj);
void squirrel_vm_add_ref(SquirrelVm* self, SquirrelObj* po);
gboolean squirrel_vm_release(SquirrelVm* self, SquirrelObj* po);
gulong squirrel_vm_get_ref_count(SquirrelVm* self, SquirrelObj* po);
void squirrel_reset_object(SquirrelObj* po);
const gchar* squirrel_obj_to_string(const SquirrelObj* po);
gboolean squirrel_obj_to_bool(const SquirrelObj* po);
glong squirrel_obj_to_integer(const SquirrelObj* po);
gfloat squirrel_obj_to_float(const SquirrelObj* po);
gpointer squirrel_obj_to_user_pointer(const SquirrelObj* po);
glong squirrel_obj_to_type_tag(const SquirrelObj* po, gpointer* typetag);
gulong squirrel_vm_get_vm_ref_count(SquirrelVm* self, const SquirrelObj* po);

// GC
glong squirrel_vm_collect_garbage(SquirrelVm* self);
glong squirrel_vm_resurrect_unreachable(SquirrelVm* self);


// serialisation
/**
 * squirrel_vm_write_closure:
 * @self: the self
 * @wp: (scope notified): the write function
 * @user_pointer: user data
 */
glong squirrel_vm_write_closure(
        SquirrelVm* self, SquirrelWriteFunc wp, gpointer user_pointer);

/**
 * squirrel_vm_read_closure:
 * @self: the self
 * @rp: (scope notified): the read function
 * @user_pointer: user data
 */
glong squirrel_vm_read_closure(
        SquirrelVm* self, SquirrelReadFunc rp, gpointer user_pointer);


glong squirrel_vm_register_mathlib(SquirrelVm* self);

G_END_DECLS




#endif

