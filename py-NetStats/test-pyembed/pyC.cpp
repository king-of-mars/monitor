/*!
Problem so far: Can't get values back from the functions ran.
*/

//Has to be included first
#include "Python.h"

#include <vector>
#include <string>
#include <assert.h>

using namespace std;

PyObject* ExecFile(PyObject* pArgs)
{
	char* FileName = NULL;
	if (!PyArg_ParseTuple(pArgs, "s", &FileName)) return NULL;
	
	PyObject* PyFileObject = PyFile_FromString(FileName, "r");
	if (PyFileObject == NULL) return NULL; // Let the user know the error.
	
	// Function Declration is: int PyRun_SimpleFile(FILE *fp, char *filename);
	// So where the hack should we get it a FILE* ? Therefore we have "PyFile_AsFile".
	PyRun_SimpleFile(PyFile_AsFile(PyFileObject), FileName);
	
	Py_DECREF(PyFileObject);
	
	// Return TRUE.
	return Py_BuildValue("i", 1);
}


vector<string> getAvailableDevices()
{
	vector<string> AvDevices;
	
	Py_Initialize();
	
	PyObject* PyFileName;
	PyFileName = Py_BuildValue("(s)", "GetAvailDevices.py");
	
	PyObject *Result;
	Result = ExecFile(PyFileName);
	
	Py_Finalize();
	
	return AvDevices;	
}

float getBitsTransferredonDevice(int DeviceNo)
{
	assert(DeviceNo>0);
	
	float BitsTransferred = 0;
	
	Py_Initialize();
	
	PyObject* PyFileName;
	PyFileName = Py_BuildValue("(s)", "GetNBytesOnDevice.py");
	
	ExecFile(PyFileName);
	
	Py_Finalize();
	
	return BitsTransferred;	
}


int main(int argc, char **argv)
{
	getAvailableDevices();
	getBitsTransferredonDevice(1);
}
