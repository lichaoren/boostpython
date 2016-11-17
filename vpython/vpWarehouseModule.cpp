#include "vpWarehouseModule.hpp"
#include <boost/python.hpp>
#include "vString.hpp"
#include "vlBase.hpp"
#include <boost/python/stl_iterator.hpp>
#include <list>
#include <vector>


// vString to python string converter 
struct vString_to_python_str {
    static PyObject* convert(vString const& s) {
        return boost::python::incref(
                boost::python::object(
                s.c_str()).ptr());
    }
};
struct vString_from_python_str {
    vString_from_python_str() {
        boost::python::converter::registry::push_back(
                &convertible,
                &construct,
                boost::python::type_id<vString>());
    }

    // Determine if obj_ptr can be converted in a QString
    static void* convertible(PyObject* obj_ptr) {
        if (!PyString_Check(obj_ptr)) return 0;
        return obj_ptr;
    }

    // Convert obj_ptr into a QString
    static void construct(
            PyObject* obj_ptr,
            boost::python::converter::rvalue_from_python_stage1_data* data) {
        // Extract the character data from the python string
        const char* value = PyString_AsString(obj_ptr);

        // Verify that obj_ptr is a string (should be ensured by convertible())
        assert(value);

        // Grab pointer to memory into which to construct the new vString
        void* storage = (
                (boost::python::converter::rvalue_from_python_storage<vString>*)
                data)->storage.bytes;

        // in-place construct the new vString using the character data
        // extraced from the python object
        new (storage) vString(value);

        // Stash the memory chunk pointer for later use by boost.python
        data->convertible = storage;
    }
};


void initializevStringConverters() {
    using namespace boost::python;

    // register the to-python converter
    to_python_converter<
            vString,
            vString_to_python_str>();

    // register the from-python converter
    vString_from_python_str();
}



// convert Vect2i,f, d to python list [x0,x1]
template<class T>
struct Vect2_to_python {
    static PyObject* convert(const Vect2<T> & p) {
        boost::python::list val;
        val.append(p[0]);
        val.append(p[1]);
        return boost::python::incref(
                boost::python::object(
                val).ptr());
    }
};

// convert Vect3i,f, d to python list [x0,x1]
template<class T>
struct Vect3_to_python {
    static PyObject* convert(const Vect3<T> & p) {
        boost::python::list val;
        val.append(p[0]);
        val.append(p[1]);
        val.append(p[2]);
        return boost::python::incref(
                boost::python::object(
                val).ptr());
    }
};
// convert Vect4i,f, d to python tuple (x0,x1, x2, x3)
template<class T>
struct Vect4_to_python {
    static PyObject* convert(const Vect4<T> & p) {
        boost::python::list val;
        for (int i = 0; i < 4; i++)
            val.append(p[i]);
        return boost::python::incref(
                boost::python::object(
                val).ptr());
    }
};

struct Vect2_from_python_tuple {
    template<class T>
    Vect2_from_python_tuple& from_python() {
        boost::python::converter::registry::push_back(
                &convertible,
                &construct< T >,
                boost::python::type_id< Vect2<T> >());
        return *this;
    }
    // Determine if obj_ptr can be converted in a Vect2
    static void* convertible(PyObject* obj_ptr) {
        if (!PyList_Check(obj_ptr)) return 0; // must be tuple
        if (PyList_Size(obj_ptr) != 2) return 0; // must has two element
        return obj_ptr;
    }
    // Convert obj_ptr into a Vect2 instance
    template<class T>
    static void construct(
            PyObject* obj_ptr,
            boost::python::converter::rvalue_from_python_stage1_data* data) {
        // Object is a borrowed reference, so create a handle indicting it is
        // borrowed for proper reference counting.
        namespace python = boost::python;
        //python::handle<> handle(python::borrowed(obj_ptr));
        //convert pyobj to boost obj and increment the ref 
        python::object o(python::handle<>(python::borrowed(obj_ptr)));
        // Obtain a handle to the memory block that the converter has allocated
        // for the C++ type.
        typedef python::converter::rvalue_from_python_storage< Vect2<T> >
                storage_type;
        void* storage = reinterpret_cast<storage_type*> (data)->storage.bytes;
        python::object lst = boost::python::extract<python::list>(o);
        data->convertible = new (storage) Vect2<T>(
                boost::python::extract<T>(lst[0]), // begin
                boost::python::extract<T>(lst[1])); // end
    }

};
// ok, we need an auto conversion mechanism between vectxy
// and python list, where x can be 2,3,4 and y can
// be i,d


struct Vect4_from_python_tuple {
    template<class T>
    Vect4_from_python_tuple& from_python() {
        boost::python::converter::registry::push_back(
                &convertible,
                &construct<T>,
                boost::python::type_id< Vect4<T> >());
        return *this;
    }

    // Determine if obj_ptr can be converted in a Vect4

    static void* convertible(PyObject* obj_ptr) {
        if (!PyList_Check(obj_ptr)) return 0; // must be tuple
        if (PyList_Size(obj_ptr) != 4) return 0; // must has two element
        return obj_ptr;
    }

    // Convert obj_ptr into a Vect2 instance

    template<class T>
    static void construct(
            PyObject* obj_ptr,
            boost::python::converter::rvalue_from_python_stage1_data* data) {
        // Object is a borrowed reference, so create a handle indicating it is
        // borrowed for proper reference counting.
        namespace python = boost::python;
        //python::handle<> handle(python::borrowed(obj_ptr));
        //convert pyobj to boost obj and increment the ref 
        python::object o(python::handle<>(python::borrowed(obj_ptr)));
        // Obtain a handle to the memory block that the converter has allocated
        // for the C++ type.
        typedef python::converter::rvalue_from_python_storage< Vect4<T> >
                storage_type;
        void* storage = reinterpret_cast<storage_type*> (data)->storage.bytes;
        python::object lst = boost::python::extract<python::list>(o);
        data->convertible = new (storage) Vect4<T>(
                boost::python::extract<T>(lst[0]), // begin
                boost::python::extract<T>(lst[1]),
                boost::python::extract<T>(lst[2])
                ); // end
    }

};


struct Vect3_from_python_tuple {
    template<class T>
    Vect3_from_python_tuple& from_python() {
        boost::python::converter::registry::push_back(
                &convertible,
                &construct<T>,
                boost::python::type_id< Vect3<T> >());
        return *this;
    }

    // Determine if obj_ptr can be converted in a Vect3

    static void* convertible(PyObject* obj_ptr) {
        if (!PyList_Check(obj_ptr)) return 0; // must be tuple
        if (PyList_Size(obj_ptr) != 3) return 0; // must has two element
        return obj_ptr;
    }

    // Convert obj_ptr into a Vect3 instance

    template<class T>
    static void construct(
            PyObject* obj_ptr,
            boost::python::converter::rvalue_from_python_stage1_data* data) {
        // Object is a borrowed reference, so create a handle indicating it is
        // borrowed for proper reference counting.
        namespace python = boost::python;
        //python::handle<> handle(python::borrowed(obj_ptr));
        //convert pyobj to boost obj and increment the ref 
        python::object o(python::handle<>(python::borrowed(obj_ptr)));
        // Obtain a handle to the memory block that the converter has allocated
        // for the C++ type.
        typedef python::converter::rvalue_from_python_storage< Vect3<T> >
                storage_type;
        void* storage = reinterpret_cast<storage_type*> (data)->storage.bytes;
        python::object lst = boost::python::extract<python::list>(o);
        data->convertible = new (storage) Vect3<T>(
                boost::python::extract<T>(lst[0]), // begin
                boost::python::extract<T>(lst[1]),
                boost::python::extract<T>(lst[2])
                ); // end
    }

};

/*-----------------------------------------------------------------------*/

vpWarehouseModule::vpWarehouseModule(const char* package)
{
    using namespace boost::python;

    //map warehouse namespace to a (sub)module
    //make "from [package.]warehouse import *" work
    std::string modulename = (package) ? std::string(package)+".warehouse"
                                       : std::string("warehouse");
    object warehouseModule(handle<>(borrowed(PyImport_AddModule(modulename.c_str()))));
    
    //make "[from package ]import warehouse" work
    //as well as import [package.]warehouse
    scope().attr("warehouse") = warehouseModule;

    //need these parentheses for the scoping of the following definitions
    //(so that they are properly "placed inside" warehouse module)
    {
        //change scope to warehouse to add definitions within warehouse module
        scope warehouse_scope = warehouseModule;

        //add classes in warehouse package
        initializevStringConverters();
        Vect2_from_python_tuple()
                .from_python<int>()
                .from_python<float>()
                .from_python<double>();
        Vect3_from_python_tuple()
                .from_python<int>()
                .from_python<float>()
                .from_python<double>();
        Vect4_from_python_tuple()
                .from_python<int>()
                .from_python<float>()
                .from_python<double>();
        to_python_converter<
                Vect2i,
                Vect2_to_python<int> >();
        to_python_converter<
                Vect2f,
                Vect2_to_python<float> >();
        to_python_converter<
                Vect2d,
                Vect2_to_python<double> >();
        to_python_converter<
                Vect3i,
                Vect3_to_python<int> >();
        to_python_converter<
                Vect3f,
                Vect3_to_python<float> >();
        to_python_converter<
                Vect3d,
                Vect3_to_python<double> >();
        to_python_converter<
                Vect4i,
                Vect4_to_python<int> >();
        to_python_converter<
                Vect4f,
                Vect4_to_python<float> >();
        to_python_converter<
                Vect4d,
                Vect4_to_python<double> >();
    }
}



