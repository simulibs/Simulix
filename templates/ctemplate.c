#include <stdlib.h>
#include <stdio.h>
#include "capi_utils.h"
#include "cJSON.h"

#define NAME_LENGTH_MAX             63 //The currently longest variable name according to Mathworks (ref/namelengthmax)

#define GetNumInputs(mmi)            rtwCAPI_GetNumRootInputs(mmi)
#define GetNumParameters(mmi)        rtwCAPI_GetNumModelParameters(mmi)
#define GetNumOutputs(mmi)           rtwCAPI_GetNumRootOutputs(mmi)

#define GetInputs(mmi)               rtwCAPI_GetRootInputs(mmi)
#define GetParameters(mmi)           rtwCAPI_GetModelParameters(mmi)
#define GetOutputs(mmi)              rtwCAPI_GetRootOutputs(mmi)

static const char *strings[] = { "SS_DOUBLE",
                                 "SS_SINGLE", 
                                 "SS_INT8", 
                                 "SS_UINT8",
                                 "SS_INT16",
                                 "SS_UINT16",
                                 "SS_INT32",
                                 "SS_UINT32",
                                 "SS_BOOLEAN"
};

#define GetDataType(num)              strings[num]

int main(int argc, const char *argv[]) {

    bil_initialize();
    bil_step();
    rtwCAPI_ModelMappingInfo* capiMap = &(rtmGetDataMapInfo(REAL_TIME_MODEL).mmi);
    


    FILE *f = fopen("ModelOutputs.json", "w");
    if(!f){
        printf("Error opening file");
        exit(13);
    }
    
    const int numRootInputs = GetNumInputs(capiMap);
    const int numModelParameters = GetNumParameters(capiMap);
    const int numRootOutputs = GetNumOutputs(capiMap);
    struct ScalarVariable sVariable;
    char name[NAME_LENGTH_MAX] = "";
    int i;
    char *string = NULL;

    cJSON *root = cJSON_CreateObject();
    cJSON *fmt = NULL;

    for (i=0; i < numRootInputs; i++) {
        sVariable = GetVariable(capiMap, i, ROOT_INPUT_FLAG);
        cJSON_AddItemToObject(root, "Input", fmt = cJSON_CreateObject());
        cJSON_AddStringToObject(fmt, "name", sVariable.name);
        cJSON_AddNumberToObject(fmt, "start", sVariable.value);
        cJSON_AddStringToObject(fmt, "type", GetDataType(sVariable.DataID));
    }

    for (i=0; i < numModelParameters; i++) {
        sVariable = GetVariable(capiMap, i, MODEL_PARAMETER_FLAG);
        cJSON_AddItemToObject(root, "Parameter", fmt = cJSON_CreateObject());
        cJSON_AddStringToObject(fmt, "name", sVariable.name);
        cJSON_AddNumberToObject(fmt, "start", sVariable.value);
        cJSON_AddStringToObject(fmt, "type", GetDataType(sVariable.DataID));
    }

    for (i=0; i < numRootOutputs; i++) {
        sVariable = GetVariable(capiMap, i, ROOT_OUTPUT_FLAG);
        cJSON_AddItemToObject(root, "Output", fmt = cJSON_CreateObject());
        cJSON_AddStringToObject(fmt, "name", sVariable.name);
        cJSON_AddNumberToObject(fmt, "start", sVariable.value);
        cJSON_AddStringToObject(fmt, "type", GetDataType(sVariable.DataID));
    }

    string = cJSON_Print(root);
    if (string == NULL) {
        fprintf(stderr, "Failed to print cJSON.\n");
        exit(12);
    }

    cJSON_Delete(root);
    fprintf(f, "%s",string);

    bil_terminate();

    fclose(f);
    return 0;
}