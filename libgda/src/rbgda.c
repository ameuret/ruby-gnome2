
/*
 * Copyright (C) 2003 Laurent Sansonetti <laurent@datarescue.be>
 *
 * This file is part of Ruby/Libgda.
 *
 * Ruby/Libgda is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Ruby/Libgda is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Ruby/Libgda; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#include "rbgda.h"

/*
 *  Module: Gda
 *
 *  The Gda module.
 */

VALUE mGda;

static void Init_all_classes(void) {
    extern void Init_gda_client(void);
    extern void Init_gda_command(void);
    extern void Init_gda_connection(void);
    extern void Init_gda_datamodel(void);
    extern void Init_gda_datamodel_array(void);
    extern void Init_gda_datamodel_hash(void);
    extern void Init_gda_datamodel_list(void);
    extern void Init_gda_datasource(void);
    extern void Init_gda_error(void);
    extern void Init_gda_parameter(void);
    extern void Init_gda_parameter_list(void);
    extern void Init_gda_provider(void);
    extern void Init_gda_row(void);
    extern void Init_gda_transaction(void);
    extern void Init_gda_value(void);

    Init_gda_client();
    Init_gda_command();
    Init_gda_connection();
    Init_gda_datamodel();
    Init_gda_datamodel_array();
    Init_gda_datamodel_hash();
    Init_gda_datamodel_list();
    Init_gda_datasource();
    Init_gda_error();
    Init_gda_parameter();
    Init_gda_parameter_list();
    Init_gda_provider();
    Init_gda_row();
    Init_gda_transaction();
    Init_gda_value();
}

/*
 *  Class method: init(appId, appVersion, *args) -> nil
 *
 *  Initializes the libgda library in the current application, 
 *  identified by appId and appVersion (both String), with given
 *  parameters.
 *
 *  If *args are ommited, libgda will be initialized using arguments
 *  from the Ruby command line.
 *
 *  Always returns nil.
 */
extern VALUE rb_progname, rb_argv;
static VALUE rb_gda_init(argc, argv, self)
    int argc;
    VALUE *argv;
    VALUE self;
{
    VALUE argary, app_id, app_version;
    gint i, gargc;
    gchar** gargv;

    /*
     *  Convert arguments in a Ruby array.
     *  If no arguments are provided, we use the command line ones.
     */
    rb_scan_args(argc, argv, "21", &app_id, &app_version, &argary);

    if (NIL_P(argary)){
        gargc = RARRAY(rb_argv)->len;
        argary = rb_argv;
    } 
    else {
        Check_Type(argary, T_ARRAY);
        gargc = RARRAY(argary)->len;
    }

    /*
     *  Fill the gargv array with the provided arguments.
     */
    gargv = ALLOCA_N(char*, gargc + 1);
    gargv[0] = (gchar *) STR2CSTR(rb_progname);

    for (i = 0; i < gargc; i++) {
        if (TYPE(RARRAY(argary)->ptr[i]) == T_STRING) {
            gargv[i+1] = (gchar *) STR2CSTR(RARRAY(argary)->ptr[i]);
        }
        else {
            gargv[i+1] = "";
        }
    }
    gargc++;

    /*
     *  Now, we can initialize libgda. 
     */
    gda_init(RVAL2CSTR(app_id), RVAL2CSTR(app_version), gargc, gargv);
    return Qnil;
}

/*
 *  Class method: main { ... } -> nil
 *
 *  Runs the GDA main loop, which is nothing more than the Bonobo main loop, 
 *  but with internally added stuff specific for applications using libgda.
 *
 *  You can specify a block code to be called after everything has been 
 *  correctly initialized (that is, for initializing your own stuff).
 *  In this case, Gda.main_quit method will be called at the end of the block.
 *
 *  Always returns nil.
 */

static VALUE __gda_callback;
static void __gda_main(data)
    gpointer data;
{
    if (!NIL_P(__gda_callback)) {
        rb_funcall(__gda_callback,
                   rb_intern("call"),
                   0);
        gda_main_quit();
    }
}

static VALUE rb_gda_main(self)
    VALUE self;
{
    __gda_callback = rb_block_given_p() ? G_BLOCK_PROC() : Qnil;
    gda_main_run(__gda_main, NULL);
    return Qnil;
}

static VALUE rb_gda_main_quit(self)
    VALUE self;
{
    gda_main_quit();
    return Qnil;
}

static VALUE rb_gda_sql_replace_placeholders(self, text, plist)
    VALUE text, plist;
{
    const gchar *sql = gda_sql_replace_placeholders(RVAL2CSTR(text), 
                                                    RGDA_PARAMETER_LIST(plist));
    return sql != NULL
        ? CSTR2RVAL(sql)
        : Qnil;
}

void Init_libgda(void) {
    /* Create the module. */
    mGda = rb_define_module("Gda");
    rb_define_module_function(mGda, "init", rb_gda_init, -1);
    rb_define_module_function(mGda, "main", rb_gda_main,  0);
    rb_define_module_function(mGda, "main_quit", rb_gda_main_quit, 0);
    rb_define_module_function(mGda, "sql_replace_placeholders",
                              rb_gda_sql_replace_placeholders, 2);

    /* Initialize all subclasses. */
    Init_all_classes();
}

