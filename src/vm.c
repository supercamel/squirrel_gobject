#include "squirrel-0.1.h"
#include <squirrel.h>
#include <sqstdmath.h>
#include <sqstdio.h>
#include <stdio.h>



SquirrelVm* squirrel_vm_copy(SquirrelVm*r)
{
    return g_memdup(r, sizeof(SquirrelVm));
}

void squirrel_vm_free(SquirrelVm*r)
{
    g_free(r);
}


// In the source
G_DEFINE_BOXED_TYPE (SquirrelVm, squirrel_vm,
                     squirrel_vm_copy,
                     squirrel_vm_free)

SquirrelObj* squirrel_obj_copy(SquirrelObj*r)
{
    return g_memdup(r, sizeof(SquirrelObj));
}

void squirrel_obj_free(SquirrelObj*r)
{
    g_free(r);
}

G_DEFINE_BOXED_TYPE (SquirrelObj, squirrel_obj,
                     squirrel_obj_copy,
                     squirrel_obj_free)


const char* squirrel_objecttype_to_string(SquirrelOBJECTTYPE t)
{
    switch(t)
    {
        case SQUIRREL_OBJECTTYPE_NULL:
            return "null";
        case SQUIRREL_OBJECTTYPE_INTEGER:
            return "integer";
        case SQUIRREL_OBJECTTYPE_FLOAT:
            return "float";
        case SQUIRREL_OBJECTTYPE_BOOL:
            return "bool";
        case SQUIRREL_OBJECTTYPE_STRING:
            return "string";
        case SQUIRREL_OBJECTTYPE_TABLE:
            return "table";
        case SQUIRREL_OBJECTTYPE_ARRAY:
            return "array";
        case SQUIRREL_OBJECTTYPE_USERDATA:
            return "userdata";
        case SQUIRREL_OBJECTTYPE_CLOSURE:
            return "closure";
        case SQUIRREL_OBJECTTYPE_NATIVECLOSURE:
            return "native closure";
        case SQUIRREL_OBJECTTYPE_GENERATOR:
            return "generator";
        case SQUIRREL_OBJECTTYPE_USERPOINTER:
            return "user pointer";
        case SQUIRREL_OBJECTTYPE_THREAD:
            return "thread";
        case SQUIRREL_OBJECTTYPE_FUNCPROTO:
            return "funcproto";
        case SQUIRREL_OBJECTTYPE_CLASS:
            return "class";
        case SQUIRREL_OBJECTTYPE_INSTANCE:
            return "instance";
        case SQUIRREL_OBJECTTYPE_WEAKREF:
            return "weakref";
        case SQUIRREL_OBJECTTYPE_OUTER:
            return "outer";
        default:
            return "undefined";
    }
}

SQRESULT squirrel_printfunc(HSQUIRRELVM v, const SQChar *s, ...)
{
    va_list vl;
    va_start(vl, s);
    vprintf(s, vl);
    va_end(vl);
    return 0;
}


SquirrelVm* squirrel_vm_new(glong initial_stack_size)
{
    SquirrelVm* vm = g_malloc(sizeof(SquirrelVm));
    vm->vm = sq_open(initial_stack_size);
    sq_setprintfunc(vm->vm, squirrel_printfunc, squirrel_printfunc);
    sqstd_seterrorhandlers(vm->vm);
    return vm;
}


SquirrelVm* squirrel_vm_from_hvm(HSQUIRRELVM hvm)
{
    SquirrelVm* vm = g_malloc(sizeof(SquirrelVm));
    vm->vm = hvm;
    return vm;
}

void squirrel_vm_close(SquirrelVm* self)
{
    sq_close(self->vm);
}

glong squirrel_vm_suspend(SquirrelVm* self)
{
    return sq_suspendvm(self->vm);
}

glong squirrel_vm_wake_up(SquirrelVm* self, 
        gboolean resumedret, 
        gboolean retval, 
        gboolean raiseerror, 
        gboolean throwerror)
{
    return sq_wakeupvm(self->vm, resumedret, retval, raiseerror, throwerror);
}

SquirrelVMSTATE squirrel_vm_get_state(SquirrelVm* self)
{
    return sq_getvmstate(self->vm);
}

glong squirrel_vm_get_version(SquirrelVm* self)
{
    return sq_getversion(self->vm);
}


gboolean squirrel_vm_do_file(
        SquirrelVm* self, 
        const gchar* path, 
        gboolean retval, 
        gboolean print_error)
{
    sq_pushroottable(self->vm);
    return sqstd_dofile(self->vm, path, retval, print_error);
}

void squirrel_vm_push(SquirrelVm* self, glong idx)
{
    sq_push(self->vm, idx);
}

void squirrel_vm_pop(SquirrelVm* self, gulong sz)
{
    sq_pop(self->vm, sz);
}

void squirrel_vm_pop_top(SquirrelVm* self)
{
    sq_poptop(self->vm);
}

void squirrel_vm_remove(SquirrelVm* self, glong idx)
{
    sq_remove(self->vm, idx);
}

glong squirrel_vm_get_top(SquirrelVm* self)
{
    return sq_gettop(self->vm);
}

void squirrel_vm_set_top(SquirrelVm* self, glong newtop)
{
    sq_settop(self->vm, newtop);
}

glong squirrel_vm_reserve_stack(SquirrelVm* self, glong ntop)
{
    return sq_reservestack(self->vm, ntop);
}

glong squirrel_vm_cmp(SquirrelVm* self)
{
    return sq_cmp(self->vm);
}

void squirrel_vm_move(SquirrelVm* self, SquirrelVm* dest, glong idx)
{
    sq_move(self->vm, dest->vm, idx);
}

void squirrel_vm_new_table(SquirrelVm* self)
{
    sq_newtable(self->vm);
}

void squirrel_vm_new_table_ex(SquirrelVm* self, glong initial_capacity)
{
    sq_newtableex(self->vm, initial_capacity);
}

void squirrel_vm_new_array(SquirrelVm* self, glong size)
{
    sq_newarray(self->vm, size);
}

static SQInteger squirrel_exec_native_closure(HSQUIRRELVM hvm)
{
    SquirrelVm* vm = squirrel_vm_from_hvm(hvm);

    SquirrelFunction c;
    sq_getuserpointer(hvm, -1, &c);
    sq_pop(hvm, 1);

    SQInteger result = 0;
    if(c != NULL) 
    {
        result = (*c)(vm);
    }
    else 
    {
        g_warning("could not get c function pointer from top of stack");
    }

    g_free(vm);

    return result;
}

void squirrel_vm_new_closure(SquirrelVm* self, SquirrelFunction c, gulong nfreevars)
{
    // we gotta intercept function calls so that the HSQUIRRELVM is wrapped by a SquirrelVm gobject
    // the user provided function pointer is pushed to the stack as a free variable
    // then the squirrel_exec_native_closure function is provided to squirrel . . . 
    // squirrel_exec_native_closure pulls the actual target function from the stack
    // packs the hvm into a SquirrelVm 
    // then calls the actual target function
    //
    // problem is, if the user also provides free variables, squirrel_exec_native_closure
    // can't know which one of these is the function pointer
    // so we do a sneaky stack shuffle to insert the function pointer at the start
    if(nfreevars > 0) {
        HSQOBJECT* freevars = g_malloc(sizeof(HSQOBJECT)*nfreevars);
        for(int i = 0; i < nfreevars; i++) {
            sq_getstackobj(self->vm, -1, &freevars[i]);
            sq_addref(self->vm, &freevars[i]);
            sq_pop(self->vm, 1);
        }
        sq_pushuserpointer(self->vm, c);

        for(int i = nfreevars-1; i >= 0; i--) {
            sq_pushobject(self->vm, freevars[i]);
            sq_release(self->vm, &freevars[i]);
        }
        g_free(freevars);
    }
    else {
        sq_pushuserpointer(self->vm, c);
    }
    sq_newclosure(self->vm, squirrel_exec_native_closure, nfreevars+1);
}

glong squirrel_vm_set_params_check(SquirrelVm* self, glong nparamscheck, const gchar* typemark)
{
    return sq_setparamscheck(self->vm, nparamscheck, typemark);
}

glong squirrel_vm_bind_env(SquirrelVm* self, glong idx)
{
    return sq_bindenv(self->vm, idx);
}

glong squirrel_vm_set_closure_root(SquirrelVm* self, glong idx)
{
    return sq_setclosureroot(self->vm, idx);
}

glong squirrel_vm_get_closure_root(SquirrelVm* self, glong idx)
{
    return sq_getclosureroot(self->vm, idx);
}

void squirrel_vm_push_string(SquirrelVm* self, const gchar* s)
{
    sq_pushstring(self->vm, s, strlen(s));
}

void squirrel_vm_push_string_with_len(SquirrelVm* self, const gchar* s, glong len)
{
    sq_pushstring(self->vm, s, len);
}

void squirrel_vm_push_int(SquirrelVm* self, glong n)
{
    sq_pushinteger(self->vm, n);
}

void squirrel_vm_push_float(SquirrelVm* self, float f) 
{
    sq_pushfloat(self->vm, f);
}

void squirrel_vm_push_bool(SquirrelVm* self, gboolean b)
{
    sq_pushbool(self->vm, b);
}

void squirrel_vm_push_user_pointer(SquirrelVm* self, gpointer p)
{
    sq_pushuserpointer(self->vm, p);
}

void squirrel_vm_push_null(SquirrelVm* self)
{
    sq_pushnull(self->vm);
}

void squirrel_vm_new_thread(SquirrelVm* self, glong initial_stack_sz)
{
    sq_newthread(self->vm, initial_stack_sz);
}

void squirrel_vm_push_thread(SquirrelVm* self, SquirrelVm* thread)
{
    sq_pushthread(self->vm, thread->vm);
}

SquirrelOBJECTTYPE squirrel_vm_get_object_type(SquirrelVm* self, glong idx)
{
    return sq_gettype(self->vm, idx);
}

glong squirrel_vm_get_typeof(SquirrelVm* self, glong idx)
{
    return sq_typeof(self->vm, idx);
}

glong squirrel_vm_get_size(SquirrelVm* self, glong idx)
{
    return sq_getsize(self->vm, idx);
}

glong squirrel_vm_get_hash(SquirrelVm* self, glong idx)
{
    return sq_gethash(self->vm, idx);
}

glong squirrel_vm_get_base(SquirrelVm* self, glong idx)
{
    return sq_getbase(self->vm, idx);
}

gboolean squirrel_vm_instance_of(SquirrelVm* self)
{
    return sq_instanceof(self->vm);
}

glong squirrel_vm_to_string(SquirrelVm* self, glong idx)
{
    return sq_tostring(self->vm, idx);
}

void* squirrel_vm_new_user_data(SquirrelVm* self, guint sz)
{
    return sq_newuserdata(self->vm, sz);
}

void squirrel_vm_to_bool(SquirrelVm* self, glong idx, gboolean* b)
{
    sq_tobool(self->vm, idx, b);
}


glong squirrel_vm_get_string(SquirrelVm* self, glong idx, gchar** s)
{
    glong ret = sq_getstring(self->vm, idx, s);
    return ret;
}

glong squirrel_vm_get_string_and_size(SquirrelVm* self, glong idx, gchar** s, glong* sz)
{
    return sq_getstringandsize(self->vm, idx, s, sz);
}

glong squirrel_vm_get_int(SquirrelVm* self, glong idx, glong* i) 
{
    return sq_getinteger(self->vm, idx, i);
}

glong squirrel_vm_get_float(SquirrelVm* self, glong idx, gfloat* f) 
{
    return sq_getfloat(self->vm, idx, f);
}

glong squirrel_vm_get_bool(SquirrelVm* self, glong idx, gboolean* b) 
{
    return sq_getbool(self->vm, idx, b);
}

glong squirrel_vm_get_thread(SquirrelVm* self, glong idx, SquirrelVm** thread)
{
    *thread = g_malloc(sizeof(SquirrelVm));
    return sq_getthread(self->vm, idx, &(*thread)->vm);
}

glong squirrel_vm_get_user_pointer(SquirrelVm* self, glong idx, gpointer* ptr)
{
    return sq_getuserpointer(self->vm, idx, ptr);
}

glong squirrel_vm_get_user_data(
        SquirrelVm* self, 
        glong idx, 
        gpointer* ptr, 
        gpointer* typetag)
{
    return sq_getuserdata(self->vm, idx, ptr, typetag);
}

glong squirrel_vm_set_type_tag(SquirrelVm* self, glong idx, gpointer typetag)
{
    return sq_settypetag(self->vm, idx, typetag);
}

glong squirrel_vm_get_type_tag(SquirrelVm* self, glong idx, gpointer* typetag)
{
    return sq_gettypetag(self->vm, idx, typetag);
}

void squirrel_vm_set_vm_release_hook(SquirrelVm* self, SquirrelReleaseHook hook)
{
    sq_setvmreleasehook(self->vm, hook);
}

gpointer squirrel_vm_get_vm_release_hook(SquirrelVm* self)
{
    return sq_getvmreleasehook(self->vm);
}

void squirrel_vm_set_foreign_pointer(SquirrelVm* self, gpointer ptr)
{
    sq_setforeignptr(self->vm, ptr);
}

gpointer squirrel_vm_get_foreign_pointer(SquirrelVm* self)
{
    return sq_getforeignptr(self->vm);
}

void squirrel_vm_set_release_hook(SquirrelVm* self, glong idx, SquirrelReleaseHook hook)
{
    sq_setreleasehook(self->vm, idx, hook);
}

SquirrelReleaseHook squirrel_vm_get_release_hook(SquirrelVm* self, glong idx)
{
    return sq_getreleasehook(self->vm, idx);
}

gchar* squirrel_vm_get_scratch_pad(SquirrelVm* self, glong idx)
{
    return sq_getscratchpad(self->vm, idx);
}

glong squirrel_vm_get_function_info(
        SquirrelVm* self, glong level, SquirrelFunctionInfo* fi)
{
    SQFunctionInfo info;
    glong r = sq_getfunctioninfo(self->vm, level, &info);

    fi = g_malloc(sizeof(SquirrelFunctionInfo));
    fi->funcid = info.funcid;
    fi->name = info.name;
    fi->source = info.source;
    fi->line = info.line;
    return r;
}

glong squirrel_vm_get_closure_info(
        SquirrelVm* self, glong idx, gulong* nparams, gulong* nfreevars)
{
    return sq_getclosureinfo(self->vm, idx, nparams, nfreevars);
}

glong squirrel_vm_get_closure_name(SquirrelVm* self, glong idx)
{
    return sq_getclosurename(self->vm, idx);
}

glong squirrel_vm_set_native_closure_name(
        SquirrelVm* self, glong idx, const gchar* name)
{
    return sq_setnativeclosurename(self->vm, idx, name);
}

glong squirrel_vm_set_instance_up(SquirrelVm* self, glong idx, gpointer p)
{
    return sq_setinstanceup(self->vm, idx, p);
}

glong squirrel_vm_get_instance_up(
        SquirrelVm* self, glong idx, gpointer* p, gpointer typetag, gboolean throwerror)
{
    return sq_getinstanceup(self->vm, idx, p, typetag, throwerror);
}

gpointer squirrel_vm_get_instance(SquirrelVm* self, glong idx)
{
    gpointer r;
    sq_getinstanceup(self->vm, idx, &r, NULL, FALSE);
    return r;
}

glong squirrel_vm_set_class_ud_size(SquirrelVm* self, glong idx, glong udsize)
{
    return sq_setclassudsize(self->vm, idx, udsize);
}

glong squirrel_vm_new_class(SquirrelVm* self, gboolean hasbase)
{
    return sq_newclass(self->vm, hasbase);
}

glong squirrel_vm_create_instance(SquirrelVm* self, glong idx)
{
    return sq_createinstance(self->vm, idx);
}

glong squirrel_vm_get_class(SquirrelVm* self, glong idx)
{
    return sq_getclass(self->vm, idx);
}

void squirrel_vm_weak_ref(SquirrelVm* self, glong idx)
{
    sq_weakref(self->vm, idx);
}

glong squirrel_vm_get_default_delegate(SquirrelVm* self, SquirrelOBJECTTYPE t)
{
    return sq_getdefaultdelegate(self->vm, t);
}

glong squirrel_vm_get_member_handle(
        SquirrelVm* self, glong idx, SquirrelMemberHandle* handle)
{
    HSQMEMBERHANDLE sqhandle;
    glong r = sq_getmemberhandle(self->vm, idx, &sqhandle);
    handle->_static = sqhandle._static;
    handle->_index = sqhandle._index;
    return r;
}

glong squirrel_vm_get_by_handle(
        SquirrelVm* self, glong idx, const SquirrelMemberHandle* handle)
{
    HSQMEMBERHANDLE sqhandle;
    sqhandle._static = handle->_static;
    sqhandle._index = handle->_index;
    return sq_getbyhandle(self->vm, idx, &sqhandle);
}

glong squirrel_vm_set_by_handle(
        SquirrelVm* self, glong idx, const SquirrelMemberHandle* handle)
{
    HSQMEMBERHANDLE sqhandle;
    sqhandle._static = handle->_static;
    sqhandle._index = handle->_index;
    return sq_setbyhandle(self->vm, idx, &sqhandle);
}



void squirrel_vm_push_root_table(SquirrelVm* self)
{
    sq_pushroottable(self->vm);
}

void squirrel_vm_push_registry_table(SquirrelVm* self)
{
    sq_pushregistrytable(self->vm);
}

void squirrel_vm_push_const_table(SquirrelVm* self)
{
    sq_pushconsttable(self->vm);
}

glong squirrel_vm_set_root_table(SquirrelVm* self)
{
    return sq_setroottable(self->vm);
}

glong squirrel_vm_set_const_table(SquirrelVm* self)
{
    return sq_setconsttable(self->vm);
}

glong squirrel_vm_new_slot(SquirrelVm* self, glong idx, gboolean bstatic)
{
    return sq_newslot(self->vm, idx, bstatic);
}

glong squirrel_vm_delete_slot(SquirrelVm* self, glong idx, gboolean pushval)
{
    return sq_deleteslot(self->vm, idx, pushval);
}

glong squirrel_vm_set(SquirrelVm* self, glong idx)
{
    return sq_set(self->vm, idx);
}

glong squirrel_vm_get(SquirrelVm* self, glong idx)
{
    return sq_get(self->vm, idx);
}

glong squirrel_vm_raw_set(SquirrelVm* self, glong idx)
{
    return sq_rawset(self->vm, idx);
}

glong squirrel_vm_raw_get(SquirrelVm* self, glong idx)
{
    return sq_rawget(self->vm, idx);
}

glong squirrel_vm_raw_delete_slot(SquirrelVm* self, glong idx, gboolean pushval)
{
    return sq_rawdeleteslot(self->vm, idx, pushval);
}

glong squirrel_vm_new_member(SquirrelVm* self, glong idx, gboolean bstatic)
{
    return sq_newmember(self->vm, idx, bstatic);
}

glong squirrel_vm_raw_new_member(SquirrelVm* self, glong idx, gboolean bstatic)
{
    return sq_rawnewmember(self->vm, idx, bstatic);
}

glong squirrel_vm_array_append(SquirrelVm* self, glong idx)
{
    return sq_arrayappend(self->vm, idx);
}

glong squirrel_vm_array_pop(SquirrelVm* self, glong idx, gboolean pushval)
{
    return sq_arraypop(self->vm, idx, pushval);
}

glong squirrel_vm_array_resize(SquirrelVm* self, glong idx, glong newsize)
{
    return sq_arrayresize(self->vm, idx, newsize);
}

glong squirrel_vm_array_reverse(SquirrelVm* self, glong idx)
{
    return sq_arrayreverse(self->vm, idx);
}

glong squirrel_vm_array_remove(SquirrelVm* self, glong idx, glong itemidx)
{
    return sq_arrayremove(self->vm, idx, itemidx);
}

glong squirrel_vm_array_insert(SquirrelVm* self, glong idx, glong destpos)
{
    return sq_arrayinsert(self->vm, idx, destpos);
}

glong squirrel_vm_set_delegate(SquirrelVm* self, glong idx)
{
    return sq_setdelegate(self->vm, idx);
}

glong squirrel_vm_get_delegate(SquirrelVm* self, glong idx)
{
    return sq_getdelegate(self->vm, idx);
}

glong squirrel_vm_clone(SquirrelVm* self, glong idx)
{
    return sq_clone(self->vm, idx);
}

glong squirrel_vm_set_free_variable(SquirrelVm* self, glong idx, gulong nval)
{
    return sq_setfreevariable(self->vm, idx, nval);
}

glong squirrel_vm_next(SquirrelVm* self, glong idx)
{
    return sq_next(self->vm, idx);
}

glong squirrel_vm_get_weak_ref_val(SquirrelVm* self, glong idx)
{
    return sq_getweakrefval(self->vm, idx);
}

glong squirrel_vm_clear(SquirrelVm* self, glong idx)
{
    return sq_clear(self->vm, idx);
}

glong squirrel_vm_call(
        SquirrelVm* self, 
        glong params, 
        gboolean retval, 
        gboolean raiseerror)
{
    return sq_call(self->vm, params, retval, raiseerror);
}

glong squirrel_vm_resume(SquirrelVm* self, gboolean retval, gboolean raiseerror)
{
    return sq_resume(self->vm, retval, raiseerror);
}

gchar* squirrel_vm_get_local(SquirrelVm* self, gulong level, gulong idx)
{
    return g_strdup(sq_getlocal(self->vm, level, idx));
}

glong squirrel_vm_get_callee(SquirrelVm* self)
{
    return sq_getcallee(self->vm);
}

gchar* squirrel_vm_get_free_variable(SquirrelVm* self, glong idx, gulong nval)
{
    return g_strdup(sq_getfreevariable(self->vm, idx, nval));
}

glong squirrel_vm_throw_error(SquirrelVm* self, const gchar* err)
{
    return sq_throwerror(self->vm, err);
}

glong squirrel_vm_throw_object(SquirrelVm* self)
{
    return sq_throwobject(self->vm);
}

void squirrel_vm_reset_error(SquirrelVm* self)
{
    sq_reseterror(self->vm);
}

void squirrel_vm_get_last_error(SquirrelVm* self)
{
    sq_getlasterror(self->vm);
}

glong squirrel_vm_get_stack_object(SquirrelVm* self, glong idx, SquirrelObj** po)
{
    *po = g_malloc(sizeof(SquirrelObj));
    return sq_getstackobj(self->vm, idx, &(*po)->hsq_obj);
}

void squirrel_vm_push_object(SquirrelVm* self, SquirrelObj* obj)
{
    sq_pushobject(self->vm, obj->hsq_obj);
}

void squirrel_vm_add_ref(SquirrelVm* self, SquirrelObj* po)
{
    sq_addref(self->vm, &po->hsq_obj);
}

gboolean squirrel_vm_release(SquirrelVm* self, SquirrelObj* po)
{
    return sq_release(self->vm, &po->hsq_obj);
}

gulong squirrel_vm_get_ref_count(SquirrelVm* self, SquirrelObj* po)
{
    return sq_getrefcount(self->vm, &po->hsq_obj);
}

void squirrel_reset_object(SquirrelObj* po)
{
    sq_resetobject(&po->hsq_obj);
}

const gchar* squirrel_obj_to_string(const SquirrelObj* obj)
{
    return sq_objtostring(&obj->hsq_obj);
}

gboolean squirrel_obj_to_bool(const SquirrelObj* po)
{
    return sq_objtobool(&po->hsq_obj);
}

glong squirrel_obj_to_integer(const SquirrelObj* po)
{
    return sq_objtointeger(&po->hsq_obj);
}

gfloat squirrel_obj_to_float(const SquirrelObj* po)
{
    return sq_objtofloat(&po->hsq_obj);
}

gpointer squirrel_obj_to_user_pointer(const SquirrelObj* po)
{
    return sq_objtouserpointer(&po->hsq_obj);
}

glong squirrel_get_obj_type_tag(const SquirrelObj* po, gpointer* typetag)
{
    return sq_getobjtypetag(&po->hsq_obj, typetag);
}

gulong squirrel_vm_get_vm_ref_count(SquirrelVm* self, const SquirrelObj* po)
{
    return sq_getvmrefcount(self->vm, &po->hsq_obj);
}


glong squirrel_vm_collect_garbage(SquirrelVm* self)
{
    return sq_collectgarbage(self->vm);
}

glong squirrel_vm_resurrect_unreachable(SquirrelVm* self)
{
    return sq_resurrectunreachable(self->vm);
}

glong squirrel_vm_write_closure(SquirrelVm* self, SquirrelWriteFunc wp, gpointer user_pointer)
{
    return sq_writeclosure(self->vm, wp, user_pointer);
}

glong squirrel_vm_read_closure(SquirrelVm* self, SquirrelReadFunc rp, gpointer user_pointer) {
    return sq_readclosure(self->vm, rp, user_pointer);
}

glong squirrel_vm_stack_infos(SquirrelVm* self, gint level, SquirrelStackInfos* si) {
    SQStackInfos sqsi;
    glong ret = sq_stackinfos(self->vm, level, &sqsi);
    si->funcname = sqsi.funcname;
    si->source = sqsi.source;
    si->line = sqsi.line;
    return ret;
}

glong squirrel_vm_register_mathlib(SquirrelVm* self)
{
    return sqstd_register_mathlib(self->vm);
}


