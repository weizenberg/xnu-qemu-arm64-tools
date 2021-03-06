/*
 * Copyright (c) 2020 Jonathan Afek <jonyafek@me.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <string.h>

#include "aleph_fb_dev.h"
#include "aleph_fb_mclass.h"
#include "aleph_fbuc_dev.h"
#include "kern_funcs.h"
#include "utils.h"
#include "mclass_reg.h"

#include "hw/arm/guest-services/general.h"

static uint8_t aleph_fb_meta_class_inst[ALEPH_MCLASS_SIZE];

//typedef struct {
//    void *fbdev;
//    void *parent_service;
//} StartFBParams;
//
//static StartFBParams start_fb_params;

void *get_fb_mclass_inst(void)
{
    return (void *)&aleph_fb_meta_class_inst[0];
}

//TODO: for the UC class override the external method method just for printing
//can debug with setting bp on memory access to UC vtable and return the
//usermode afterwards in GDB

//TODO: understand the process of inheritance from UC - probably the same or similar but must review first

//TODO: should probably get the IOSurfaceRoot service, save it and return it in the relevant external method/trap

//virtual functions of our driver
void *AlephFramebufferDevice_getMetaClass(void *this)
{
    return get_fb_mclass_inst();
}


//void start_new_aleph_fbdev(StartFBParams *sfb_params)
//{
//    void **vtable_ptr = (void **)*(uint64_t *)sfb_params->fbdev;
//    FuncIOSerivceStart vfunc_start =
//        (FuncIOSerivceStart)vtable_ptr[IOSERVICE_START_INDEX];
//    vfunc_start(sfb_params->fbdev, sfb_params->parent_service);
//}

void create_new_aleph_fbdev(void *parent_service)
{
    //TODO: release this object ref?
    void *fbdev = OSMetaClass_allocClassWithName(FBDEV_CLASS_NAME);
    if (NULL == fbdev) {
        cancel();
    }
    void **vtable_ptr = (void **)*(uint64_t *)fbdev;

    FuncIOServiceInit vfunc_init =
                (FuncIOServiceInit)vtable_ptr[IOSERVICE_INIT_INDEX];
    vfunc_init(fbdev, NULL);

    if (NULL == parent_service) {
        cancel();
    }
    FuncIOServiceSetProperty vfunc_sprop =
        (FuncIOServiceSetProperty)vtable_ptr[IOSERVICE_SET_PROPERTY_INDEX];
    vfunc_sprop(fbdev, "IOUserClientClass", FBUC_CLASS_NAME);

    FuncIOServiceAttach vfunc_attach =
                (FuncIOServiceAttach)vtable_ptr[IOSERVICE_ATTACH_INDEX];
    vfunc_attach(fbdev, parent_service);

    //FuncIOSerivceStart vfunc_start =
    //    (FuncIOSerivceStart)vtable_ptr[IOSERVICE_START_INDEX];
    //vfunc_start(fbdev, parent_service);
    FuncIOSerivceRegisterService vfunc_reg_service =
        (FuncIOSerivceRegisterService)vtable_ptr[IOSERVICE_REG_SERVICE_INDEX];
    vfunc_reg_service(fbdev, 0);

    //uint64_t unused;
    //start_fb_params.fbdev = fbdev;
    //start_fb_params.parent_service = parent_service;
    //kernel_thread_start(&start_new_aleph_fbdev, &start_fb_params, &unused);
}
