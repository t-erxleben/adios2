/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * JULEA engine using the JULEA storage framework to handle lower I/O.
 *
 *  Created on: Aug 01, 2019
 *      Author: Kira Duwe duwe@informatik.uni-hamburg.de
 */

#include "adios2/toolkit/interop/julea/JuleaInteraction.h"
#include "JuleaDBInteractionWriter.h"
#include "JuleaDBInteractionReader.h"
// #include "JuleaMetadata.h"

#include <assert.h>
#include <bson.h>
#include <glib.h>
#include <string.h>

#include <iostream>
#include <julea-db.h>
#include <julea-kv.h>
#include <julea-object.h>
#include <julea.h>

namespace adios2
{
namespace interop
{

JuleaDBInteractionWriter::JuleaDBInteractionWriter(helper::Comm const &comm) : JuleaInteraction(std::move(comm))
{
    std::cout << "This is the constructor" << std::endl;
}


void DAIaddFieldsForVariableMD(JDBSchema *schema)
{
    gchar const *fileIndex[] = {"file", NULL};
    gchar const *varIndex[] = {"variableName", NULL};
    gchar const *minFloatIndex[] = {"variableName", NULL};
    gchar const *maxFloatIndex[] = {"variableName", NULL};
    gchar const *minDoubleIndex[] = {"variableName", NULL};
    gchar const *maxDoubleIndex[] = {"variableName", NULL};
    gchar const *meanDoubleIndex[] = {"variableName", NULL};

    j_db_schema_add_field(schema, "file", J_DB_TYPE_STRING, NULL);
    j_db_schema_add_field(schema, "variableName", J_DB_TYPE_STRING, NULL);

    j_db_schema_add_field(schema, "isConstantDims", J_DB_TYPE_UINT32, NULL);
    j_db_schema_add_field(schema, "isReadAsJoined", J_DB_TYPE_UINT32, NULL);
    j_db_schema_add_field(schema, "isReadAsLocalValue", J_DB_TYPE_UINT32, NULL);
    j_db_schema_add_field(schema, "isRandomAccess", J_DB_TYPE_UINT32, NULL);
    j_db_schema_add_field(schema, "isSingleValue", J_DB_TYPE_UINT32, NULL);

    j_db_schema_add_field(schema, "shapeID", J_DB_TYPE_UINT32, NULL);
    // TODO: needed?
    // j_db_schema_add_field(schema, "typeString", J_DB_TYPE_STRING, NULL);
    // j_db_schema_add_field(schema, "typeLen", J_DB_TYPE_UINT64, NULL);

    // TODO: Check whether this renaming screws up anything in init
    j_db_schema_add_field(schema, "typeInt", J_DB_TYPE_UINT32, NULL);

    /** all vectors need to store their size */
    j_db_schema_add_field(schema, "shapeSize", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "shape", J_DB_TYPE_BLOB, NULL);
    j_db_schema_add_field(schema, "startSize", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "start", J_DB_TYPE_BLOB, NULL);
    j_db_schema_add_field(schema, "countSize", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "count", J_DB_TYPE_BLOB, NULL);

    /** number of blocks (steps are index starting at 0) */
    j_db_schema_add_field(schema, "numberSteps", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "blockArray", J_DB_TYPE_BLOB, NULL);

    j_db_schema_add_field(schema, "min_blob", J_DB_TYPE_BLOB, NULL);
    j_db_schema_add_field(schema, "max_blob", J_DB_TYPE_BLOB, NULL);
    j_db_schema_add_field(schema, "value_blob", J_DB_TYPE_BLOB, NULL);

    // add min/max/value for every type for performance improvement of querying
    j_db_schema_add_field(schema, "min_sint32", J_DB_TYPE_SINT32, NULL);
    j_db_schema_add_field(schema, "max_sint32", J_DB_TYPE_SINT32, NULL);
    j_db_schema_add_field(schema, "value_sint32", J_DB_TYPE_SINT32, NULL);

    j_db_schema_add_field(schema, "min_uint32", J_DB_TYPE_UINT32, NULL);
    j_db_schema_add_field(schema, "max_uint32", J_DB_TYPE_UINT32, NULL);
    j_db_schema_add_field(schema, "value_uint32", J_DB_TYPE_UINT32, NULL);

    j_db_schema_add_field(schema, "min_sint64", J_DB_TYPE_SINT64, NULL);
    j_db_schema_add_field(schema, "max_sint64", J_DB_TYPE_SINT64, NULL);
    j_db_schema_add_field(schema, "value_sint64", J_DB_TYPE_SINT64, NULL);

    j_db_schema_add_field(schema, "min_uint64", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "max_uint64", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "value_uint64", J_DB_TYPE_UINT64, NULL);

    j_db_schema_add_field(schema, "min_float32", J_DB_TYPE_FLOAT32, NULL);
    j_db_schema_add_field(schema, "max_float32", J_DB_TYPE_FLOAT32, NULL);
    j_db_schema_add_field(schema, "value_float32", J_DB_TYPE_FLOAT32, NULL);

    j_db_schema_add_field(schema, "min_float64", J_DB_TYPE_FLOAT64, NULL);
    j_db_schema_add_field(schema, "max_float64", J_DB_TYPE_FLOAT64, NULL);
    j_db_schema_add_field(schema, "value_float64", J_DB_TYPE_FLOAT64, NULL);
    j_db_schema_add_field(schema, "mean_float64", J_DB_TYPE_FLOAT64, NULL);

    // j_db_schema_add_field(schema, "min_string", J_DB_TYPE_STRING, NULL);
    // j_db_schema_add_field(schema, "max_string", J_DB_TYPE_STRING, NULL);
    j_db_schema_add_field(schema, "value_string", J_DB_TYPE_STRING, NULL);

    j_db_schema_add_index(schema, fileIndex, NULL);
    j_db_schema_add_index(schema, varIndex, NULL);
    j_db_schema_add_index(schema, minFloatIndex, NULL);
    j_db_schema_add_index(schema, maxFloatIndex, NULL);
    j_db_schema_add_index(schema, minDoubleIndex, NULL);
    j_db_schema_add_index(schema, maxDoubleIndex, NULL);
    j_db_schema_add_index(schema, meanDoubleIndex, NULL);
}

void DAIaddFieldsForBlockMD(JDBSchema *schema)
{
    gchar const *fileIndex[] = {"file", NULL};
    gchar const *varIndex[] = {"variableName", NULL};
    gchar const *stepIndex[] = {"step", NULL};
    gchar const *blockIndex[] = {"block", NULL};
    gchar const *minFloatIndex[] = {"variableName", NULL};
    gchar const *maxFloatIndex[] = {"variableName", NULL};
    gchar const *minDoubleIndex[] = {"variableName", NULL};
    gchar const *maxDoubleIndex[] = {"variableName", NULL};
    gchar const *meanDoubleIndex[] = {"variableName", NULL};
    //    gchar const *minIndex[] = {"min_blob", NULL};
    //    gchar const *maxIndex[] = {"max_blob", NULL};

    j_db_schema_add_field(schema, "file", J_DB_TYPE_STRING, NULL);
    j_db_schema_add_field(schema, "variableName", J_DB_TYPE_STRING, NULL);
    j_db_schema_add_field(schema, "step", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "block", J_DB_TYPE_UINT64, NULL);

    /** all vectors need to store their size */
    j_db_schema_add_field(schema, "shapeSize", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "shape", J_DB_TYPE_BLOB, NULL);
    j_db_schema_add_field(schema, "startSize", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "start", J_DB_TYPE_BLOB, NULL);
    j_db_schema_add_field(schema, "countSize", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "count", J_DB_TYPE_BLOB, NULL);
    j_db_schema_add_field(schema, "memoryStartSize", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "memoryStart", J_DB_TYPE_BLOB, NULL);
    j_db_schema_add_field(schema, "memoryCountSize", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "memoryCount", J_DB_TYPE_BLOB, NULL);

    j_db_schema_add_field(schema, "isValue", J_DB_TYPE_UINT32, NULL);

    j_db_schema_add_field(schema, "min_blob", J_DB_TYPE_BLOB, NULL);
    j_db_schema_add_field(schema, "max_blob", J_DB_TYPE_BLOB, NULL);
    j_db_schema_add_field(schema, "value_blob", J_DB_TYPE_BLOB, NULL);

    // add min/max/value for every type for performance improvement of querying
    j_db_schema_add_field(schema, "min_sint32", J_DB_TYPE_SINT32, NULL);
    j_db_schema_add_field(schema, "max_sint32", J_DB_TYPE_SINT32, NULL);
    j_db_schema_add_field(schema, "value_sint32", J_DB_TYPE_SINT32, NULL);

    j_db_schema_add_field(schema, "min_uint32", J_DB_TYPE_UINT32, NULL);
    j_db_schema_add_field(schema, "max_uint32", J_DB_TYPE_UINT32, NULL);
    j_db_schema_add_field(schema, "value_uint32", J_DB_TYPE_UINT32, NULL);

    j_db_schema_add_field(schema, "min_sint64", J_DB_TYPE_SINT64, NULL);
    j_db_schema_add_field(schema, "max_sint64", J_DB_TYPE_SINT64, NULL);
    j_db_schema_add_field(schema, "value_sint64", J_DB_TYPE_SINT64, NULL);

    j_db_schema_add_field(schema, "min_uint64", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "max_uint64", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "value_uint64", J_DB_TYPE_UINT64, NULL);

    j_db_schema_add_field(schema, "min_float32", J_DB_TYPE_FLOAT32, NULL);
    j_db_schema_add_field(schema, "max_float32", J_DB_TYPE_FLOAT32, NULL);
    j_db_schema_add_field(schema, "value_float32", J_DB_TYPE_FLOAT32, NULL);

    j_db_schema_add_field(schema, "min_float64", J_DB_TYPE_FLOAT64, NULL);
    j_db_schema_add_field(schema, "max_float64", J_DB_TYPE_FLOAT64, NULL);
    j_db_schema_add_field(schema, "value_float64", J_DB_TYPE_FLOAT64, NULL);
    j_db_schema_add_field(schema, "mean_float64", J_DB_TYPE_FLOAT64, NULL);

    // j_db_schema_add_field(schema, "min_string", J_DB_TYPE_STRING, NULL);
    // j_db_schema_add_field(schema, "max_string", J_DB_TYPE_STRING, NULL);
    j_db_schema_add_field(schema, "value_string", J_DB_TYPE_STRING, NULL);

    j_db_schema_add_field(schema, "stepsStart", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "stepsCount", J_DB_TYPE_UINT64, NULL);
    j_db_schema_add_field(schema, "blockID", J_DB_TYPE_UINT64, NULL);

    j_db_schema_add_index(schema, fileIndex, NULL);
    j_db_schema_add_index(schema, varIndex, NULL);
    j_db_schema_add_index(schema, stepIndex, NULL);
    j_db_schema_add_index(schema, blockIndex, NULL);
    j_db_schema_add_index(schema, minFloatIndex, NULL);
    j_db_schema_add_index(schema, maxFloatIndex, NULL);
    j_db_schema_add_index(schema, minDoubleIndex, NULL);
    j_db_schema_add_index(schema, maxDoubleIndex, NULL);
    j_db_schema_add_index(schema, meanDoubleIndex, NULL);
    //    j_db_schema_add_index(schema, minIndex, NULL);
    //    j_db_schema_add_index(schema, maxIndex, NULL);
}

template <class T>
void DAIaddEntriesForVariableMD(core::Variable<T> &variable,
                                const std::string nameSpace,
                                const std::string varName, size_t currStep,
                                JDBSchema *schema, JDBEntry *entry)
{
    bool isConstantDims = variable.IsConstantDims();
    int tmp = isConstantDims ? 1 : 0;
    bool isReadAsJoined = variable.m_ReadAsJoined;
    int tmp2 = isReadAsJoined ? 1 : 0;
    bool isReadAsLocalValue = variable.m_ReadAsLocalValue;
    int tmp3 = isReadAsLocalValue ? 1 : 0;
    bool isRandomAccess = variable.m_RandomAccess;
    int tmp4 = isRandomAccess ? 1 : 0;
    bool isSingleValue = variable.m_SingleValue;
    int tmp5 = isSingleValue ? 1 : 0;

    int shapeID = (int)variable.m_ShapeID;

    // const char *type = variable.m_Type.c_str();
    const int type = static_cast<int>(variable.m_Type);
    size_t shapeSize = variable.m_Shape.size();
    size_t startSize = variable.m_Start.size();
    size_t countSize = variable.m_Count.size();
    size_t numberSteps = currStep + 1;

    size_t shapeIDLen = sizeof(int);
    // TODO: typeLen no longer needed;
    // FIXME: wherever used
    // size_t typeLen = sizeof(variable.m_Type.c_str());
    size_t blocks[numberSteps];

    // if the entry does not exist in the map it will be added. This is actually
    // what we want here. This way, for all steps prior to the first step of
    // this variable, an element will be created of size 0
    for (uint i = 0; i < numberSteps; i++)
    {
        // std::cout << "mapsize: "
        //           << variable.m_AvailableStepBlockIndexOffsets.size()
        //           << std::endl;
        blocks[i] = variable.m_AvailableStepBlockIndexOffsets[i].size();
        // std::cout << "mapsize: "
        //           << variable.m_AvailableStepBlockIndexOffsets.size()
        //           << std::endl;
        // std::cout << "i: " << i << "  blocks: " << blocks[i] << std::endl;
    }
    if (false)
    {
        // std::cout << "typeLen: " << typeLen << std::endl;
        std::cout << "variable.m_ShapeID: " << variable.m_ShapeID << std::endl;
        std::cout << "shapeID: " << shapeID << std::endl;
        std::cout << "constantDims: " << isConstantDims << std::endl;
        std::cout << "isReadAsJoined: " << isReadAsJoined << std::endl;
        std::cout << "isReadAsLocalValue: " << isReadAsLocalValue << std::endl;
        std::cout << "isRandomAccess: " << isRandomAccess << std::endl;
        std::cout << "type: " << type << std::endl;
        std::cout << "shapeSize: " << shapeSize << std::endl;
        std::cout << "startSize: " << startSize << std::endl;
        std::cout << "countSize: " << countSize << std::endl;
        std::cout << "numberSteps: " << numberSteps << std::endl;
        std::cout << "shape.data = " << variable.m_Shape.data() << std::endl;
        std::cout << "count.data = " << variable.m_Count.data() << std::endl;
        std::cout << "numberSteps: " << numberSteps << std::endl;
        std::cout << "m_AvailableStepStart: " << variable.m_AvailableStepsStart
                  << std::endl;
        std::cout << "m_AvailableStepsCount: " << variable.m_AvailableStepsCount
                  << std::endl;
        std::cout << "m_StepsStart: " << variable.m_StepsStart << std::endl;
        std::cout << "m_StepsCount: " << variable.m_StepsCount << std::endl;
    }

    j_db_entry_set_field(entry, "file", nameSpace.c_str(),
                         strlen(nameSpace.c_str()) + 1, NULL);
    j_db_entry_set_field(entry, "variableName", varName.c_str(),
                         strlen(nameSpace.c_str()) + 1, NULL);

    j_db_entry_set_field(entry, "isConstantDims", &tmp, sizeof(tmp), NULL);
    j_db_entry_set_field(entry, "isReadAsJoined", &tmp2, sizeof(tmp2), NULL);
    j_db_entry_set_field(entry, "isReadAsLocalValue", &tmp3, sizeof(tmp3),
                         NULL);
    j_db_entry_set_field(entry, "isRandomAccess", &tmp4, sizeof(tmp4), NULL);
    j_db_entry_set_field(entry, "isSingleValue", &tmp5, sizeof(tmp5), NULL);

    j_db_entry_set_field(entry, "shapeID", &shapeID, sizeof(shapeID), NULL);
    // j_db_entry_set_field(entry, "typeString", type, strlen(type) + 1, NULL);
    j_db_entry_set_field(entry, "typeInt", &type, sizeof(type), NULL);
    // TODO typeLen no longer needed
    // j_db_entry_set_field(entry, "typeLen", &typeLen, typeLen, NULL);

    j_db_entry_set_field(entry, "shapeSize", &shapeSize, sizeof(shapeSize),
                         NULL);
    j_db_entry_set_field(
        entry, "shape", variable.m_Shape.data(),
        sizeof(*variable.m_Shape.data()) * variable.m_Shape.size(), NULL);
    j_db_entry_set_field(entry, "startSize", &startSize, sizeof(startSize),
                         NULL);
    j_db_entry_set_field(
        entry, "start", variable.m_Start.data(),
        sizeof(*variable.m_Start.data()) * variable.m_Start.size(), NULL);
    j_db_entry_set_field(entry, "countSize", &countSize, sizeof(countSize),
                         NULL);
    j_db_entry_set_field(
        entry, "count", variable.m_Count.data(),
        sizeof(*variable.m_Count.data()) * variable.m_Count.size(), NULL);

    j_db_entry_set_field(entry, "numberSteps", &numberSteps,
                         sizeof(numberSteps), NULL);
    j_db_entry_set_field(entry, "blockArray", blocks, sizeof(blocks), NULL);

    // const char *varType = variable.m_Type.c_str();
    // const int varType = static_cast<int>(variable.m_Type);
    std::string minField;
    std::string maxField;
    std::string valueField;
    std::string meanField;

    JuleaInteraction::SetMinMaxValueFields(&minField, &maxField, &valueField, &meanField,
                            variable.m_Type);

    j_db_entry_set_field(entry, minField.c_str(), &variable.m_Min,
                         sizeof(variable.m_Min), NULL);
    j_db_entry_set_field(entry, maxField.c_str(), &variable.m_Max,
                         sizeof(variable.m_Max), NULL);
}

template <class T>
void DAIaddEntriesForBlockMD(core::Variable<T> &variable, const std::string nameSpace,
                             const std::string varName, size_t currStep,
                             size_t block,
                             const typename core::Variable<T>::Info &blockInfo,
                             JDBSchema *schema, JDBEntry *entry, T &blockMin,
                             T &blockMax, T &blockMean)
{
    size_t shapeSize = variable.m_Shape.size();
    size_t startSize = variable.m_Start.size();
    size_t countSize = variable.m_Count.size();
    size_t memoryStartSize = blockInfo.MemoryStart.size();
    size_t memoryCountSize = blockInfo.MemoryCount.size();

    size_t minLen = sizeof(blockMin);
    size_t maxLen = sizeof(blockMax);
    size_t valueLen = sizeof(variable.m_Value);
    size_t meanLen = sizeof(blockMean);

    size_t stepsStart = blockInfo.StepsStart;
    size_t stepsCount = blockInfo.StepsCount;
    size_t blockID = block;
    size_t stepID = currStep;

    bool isValue = blockInfo.IsValue;
    int tmp = isValue ? 1 : 0;

    if (false)
    {
        std::cout << "shapeSize: " << shapeSize << std::endl;
        std::cout << "var: shape.data: " << variable.m_Shape.data()
                  << std::endl;
        std::cout << "blockInfo:shape.data: " << variable.m_Shape.data()
                  << std::endl;
        std::cout << "    variable minimum: " << variable.m_Min << std::endl;
        std::cout << "    variable maximum: " << variable.m_Max << std::endl;
        std::cout << "variable min size: " << minLen << std::endl;
        std::cout << "size of T: " << sizeof(T) << std::endl;
        std::cout << "stepsStart" << stepsStart << std::endl;
        std::cout << "stepsCount" << stepsCount << std::endl;
        std::cout << "blockID: " << blockID << std::endl;
    }

    j_db_entry_set_field(entry, "file", nameSpace.c_str(),
                         strlen(nameSpace.c_str()) + 1, NULL);
    j_db_entry_set_field(entry, "variableName", varName.c_str(),
                         strlen(varName.c_str()) + 1, NULL);
    j_db_entry_set_field(entry, "step", &stepID, sizeof(stepID), NULL);
    j_db_entry_set_field(entry, "block", &blockID, sizeof(blockID), NULL);

    j_db_entry_set_field(entry, "shapeSize", &shapeSize, sizeof(shapeSize),
                         NULL);
    j_db_entry_set_field(
        entry, "shape", variable.m_Shape.data(),
        sizeof(*variable.m_Shape.data()) * variable.m_Shape.size(), NULL);
    j_db_entry_set_field(entry, "startSize", &startSize, sizeof(startSize),
                         NULL);
    j_db_entry_set_field(
        entry, "start", variable.m_Start.data(),
        sizeof(*variable.m_Start.data()) * variable.m_Start.size(), NULL);
    j_db_entry_set_field(entry, "countSize", &countSize, sizeof(countSize),
                         NULL);
    j_db_entry_set_field(
        entry, "count", variable.m_Count.data(),
        sizeof(*variable.m_Count.data()) * variable.m_Count.size(), NULL);

    j_db_entry_set_field(entry, "memoryStartSize", &memoryStartSize,
                         sizeof(memoryStartSize), NULL);
    j_db_entry_set_field(entry, "memoryStart", blockInfo.MemoryStart.data(),
                         sizeof(*blockInfo.MemoryStart.data()) *
                             blockInfo.MemoryStart.size(),
                         NULL);
    j_db_entry_set_field(entry, "memoryCountSize", &memoryCountSize,
                         sizeof(memoryCountSize), NULL);
    j_db_entry_set_field(entry, "memoryCount", blockInfo.MemoryCount.data(),
                         sizeof(*blockInfo.MemoryCount.data()) *
                             blockInfo.MemoryCount.size(),
                         NULL);

    // const char *varType = variable.m_Type.c_str();
    // const int varType = static_cast<int>(variable.m_Type);
    std::string minField;
    std::string maxField;
    std::string valueField;
    std::string meanField;

    JuleaInteraction::SetMinMaxValueFields(&minField, &maxField, &valueField, &meanField,
                            variable.m_Type);

    j_db_entry_set_field(entry, minField.c_str(), &blockMin, minLen, NULL);
    j_db_entry_set_field(entry, maxField.c_str(), &blockMax, maxLen, NULL);
    j_db_entry_set_field(entry, meanField.c_str(), &blockMean, meanLen, NULL);

    j_db_entry_set_field(entry, "isValue", &tmp, sizeof(tmp), NULL);

    if (isValue)
    {
        std::cout << "Writing local value for " << varName << std::endl;
        j_db_entry_set_field(entry, valueField.c_str(), &variable.m_Value,
                             valueLen, NULL);
    }
    else
    {
        size_t value = 0;
        valueLen = sizeof(value);
        j_db_entry_set_field(entry, "value", &value, valueLen, NULL);
    }

    j_db_entry_set_field(entry, "stepsStart", &stepsStart, sizeof(stepsStart),
                         NULL);
    j_db_entry_set_field(entry, "stepsCount", &stepsCount, sizeof(stepsCount),
                         NULL);
    j_db_entry_set_field(entry, "blockID", &blockID, sizeof(blockID), NULL);
}



void JuleaDBInteractionWriter::InitDBSchemas()
{
    // std::cout << "--- InitDBSchemas ---" << std::endl;
    int err = 0;
    auto semantics = j_semantics_new(J_SEMANTICS_TEMPLATE_DEFAULT);
    auto batch = j_batch_new(semantics);
    auto batch2 = j_batch_new(semantics);
    g_autoptr(JDBSchema) varSchema = NULL;
    g_autoptr(JDBSchema) blockSchema = NULL;

    varSchema = j_db_schema_new("adios2", "variable-metadata", NULL);
    blockSchema = j_db_schema_new("adios2", "block-metadata", NULL);

    j_db_schema_get(varSchema, batch, NULL);
    bool existsVar = j_batch_execute(batch);
    j_db_schema_get(blockSchema, batch, NULL);

    bool existsBlock = j_batch_execute(batch);
    // std::cout << "existsVar: " << existsVar << " existsBlock: " <<
    // existsBlock
    // << std::endl;

    if (existsVar == 0)
    {
        // std::cout << "variable schema does not exist" << std::endl;
        varSchema = j_db_schema_new("adios2", "variable-metadata", NULL);
        DAIaddFieldsForVariableMD(varSchema);
        j_db_schema_create(varSchema, batch, NULL);
        g_assert_true(j_batch_execute(batch) == true);
    }

    if (existsBlock == 0)
    {

        // std::cout << "block schema does not exist" << std::endl;
        blockSchema = j_db_schema_new("adios2", "block-metadata", NULL);
        DAIaddFieldsForBlockMD(blockSchema);
        j_db_schema_create(blockSchema, batch2, NULL);
        g_assert_true(j_batch_execute(batch2) == true);
    }

    // g_assert_true(j_batch_execute(batch2) == true);
    // j_db_schema_unref(varSchema);
    // j_db_schema_unref(blockSchema);
    j_batch_unref(batch);
    j_batch_unref(batch2);
    j_semantics_unref(semantics);
}

template <class T>
void JuleaDBInteractionWriter::PutVariableMetadataToJulea(
    core::Variable<T> &variable, const std::string nameSpace,
    const std::string varName, size_t currStep, size_t block)
{
    int err = 0;
    g_autoptr(JDBSchema) schema = NULL;
    g_autoptr(JDBEntry) entry = NULL;
    g_autoptr(JDBIterator) iterator = NULL;
    g_autoptr(JDBSelector) selector = NULL;

    // void *namesBuf = NULL;
    auto semantics = j_semantics_new(J_SEMANTICS_TEMPLATE_DEFAULT);
    auto batch = j_batch_new(semantics);
    auto batch2 = j_batch_new(semantics);

    schema = j_db_schema_new("adios2", "variable-metadata", NULL);

    // TODO: necessary to get schema every time?
    j_db_schema_get(schema, batch, NULL);
    err = j_batch_execute(batch);

    entry = j_db_entry_new(schema, NULL);
    DAIaddEntriesForVariableMD(variable, nameSpace, varName, currStep, schema,
                               entry);

    /** check whether variable needs to be updated or inserted */
    selector = j_db_selector_new(schema, J_DB_SELECTOR_MODE_AND, NULL);
    j_db_selector_add_field(selector, "file", J_DB_SELECTOR_OPERATOR_EQ,
                            nameSpace.c_str(), strlen(nameSpace.c_str()) + 1,
                            NULL);
    j_db_selector_add_field(selector, "variableName", J_DB_SELECTOR_OPERATOR_EQ,
                            varName.c_str(), strlen(varName.c_str()) + 1, NULL);
    iterator = j_db_iterator_new(schema, selector, NULL);

    if (j_db_iterator_next(iterator, NULL))
    {
        j_db_entry_update(entry, selector, batch2, NULL);
    }
    else
    {
        // std::cout << "Variable metadata does not exist yet." << std::endl;
        j_db_entry_insert(entry, batch2, NULL);
    }

    err = j_batch_execute(batch2);
    // g_assert_true(j_batch_execute(batch2) == true);

    // j_db_entry_unref(entry);
    // j_db_schema_unref(schema);
    j_batch_unref(batch);
    j_batch_unref(batch2);
    j_semantics_unref(semantics);
}

template <class T>
void JuleaDBInteractionWriter::PutBlockMetadataToJulea(
    core::Variable<T> &variable, const std::string nameSpace,
    const std::string varName, size_t step, size_t block,
    const typename core::Variable<T>::Info &blockInfo, T &blockMin, T &blockMax,
    T &blockMean, uint32_t &entryID)
{
    int err = 0;
    g_autoptr(JDBSchema) schema = NULL;
    g_autoptr(JDBEntry) entry = NULL;
    g_autoptr(JDBSelector) selector = NULL;
    g_autoptr(JDBIterator) iterator = NULL;
    JDBType jdbType;
    guint64 db_length = 0;
    uint32_t *tmpID;
    // uint32_t entryID = 0;

    // void *namesBuf = NULL;
    auto semantics = j_semantics_new(J_SEMANTICS_TEMPLATE_DEFAULT);
    auto batch = j_batch_new(semantics);
    auto batch2 = j_batch_new(semantics);

    schema = j_db_schema_new("adios2", "block-metadata", NULL);
    j_db_schema_get(schema, batch, NULL);
    err = j_batch_execute(batch);
    // g_assert_true(j_batch_execute(batch) == true);

    entry = j_db_entry_new(schema, NULL);
    DAIaddEntriesForBlockMD(variable, nameSpace, varName, step, block,
                            blockInfo, schema, entry, blockMin, blockMax,
                            blockMean);

    /** check whether blcock needs to be updated or inserted */
    selector = j_db_selector_new(schema, J_DB_SELECTOR_MODE_AND, NULL);
    j_db_selector_add_field(selector, "file", J_DB_SELECTOR_OPERATOR_EQ,
                            nameSpace.c_str(), strlen(nameSpace.c_str()) + 1,
                            NULL);
    j_db_selector_add_field(selector, "variableName", J_DB_SELECTOR_OPERATOR_EQ,
                            varName.c_str(), strlen(varName.c_str()) + 1, NULL);
    j_db_selector_add_field(selector, "step", J_DB_SELECTOR_OPERATOR_EQ, &step,
                            sizeof(step), NULL);
    j_db_selector_add_field(selector, "block", J_DB_SELECTOR_OPERATOR_EQ,
                            &block, sizeof(block), NULL);

    iterator = j_db_iterator_new(schema, selector, NULL);

    if (j_db_iterator_next(iterator, NULL))
    {
        j_db_entry_update(entry, selector, batch2, NULL);
        err = j_batch_execute(batch2);
        // j_db_iterator_get_field(iterator, "_id", &jdbType, (gpointer
        // *)&tmpID,
        //                         &db_length, NULL);
        // std::cout << "_id: " << *tmpID << std::endl;
        // entryID = *tmpID;
    }
    else
    {
        // std::cout << "Variable metadata does not exist yet." << std::endl;
        j_db_entry_insert(entry, batch2, NULL);
        err = j_batch_execute(batch2);

        iterator = j_db_iterator_new(schema, selector, NULL);
        j_db_iterator_next(iterator, NULL);
        // j_db_iterator_get_field(iterator, "_id", &jdbType, (gpointer
        // *)&tmpID,
        //                         &db_length, NULL);

        // std::cout << "_id: " << *tmpID << std::endl;
        // entryID = *tmpID;
    }
    j_db_iterator_get_field(iterator, "_id", &jdbType, (gpointer *)&tmpID,
                            &db_length, NULL);

    // std::cout << "_id: " << *tmpID << std::endl;
    entryID = *tmpID;
    // variable->m_AvailableStepBlockIndexOffsets[step].

    // TODO: how to use this function?
    // j_db_entry_get_id(entry, )

    // g_assert_true(j_batch_execute(batch2) == true);

    // j_db_entry_unref(entry);
    // j_db_schema_unref(schema);
    j_batch_unref(batch);
    j_batch_unref(batch2);
    j_semantics_unref(semantics);
}

// template <class T>
// void DAIDBPutVariableDataToJulea(Variable<T> &variable, const T *data,
//                               const std::string nameSpace, uint32_t entryID)
// {
//     // std::cout << "--- PutVariableDataToJulea ----- " << std::endl;
//     // std::cout << "data: " << data[0] << std::endl;
//     // std::cout << "data: " << data[1] << std::endl;

//     guint64 bytesWritten = 0;
//     std::string objName = "variableblocks";

//     auto semantics = j_semantics_new(J_SEMANTICS_TEMPLATE_DEFAULT);
//     j_semantics_set(semantics, J_SEMANTICS_SAFETY,
//     J_SEMANTICS_SAFETY_STORAGE); auto batch = j_batch_new(semantics);

//     auto numberElements = adios2::helper::GetTotalSize(variable.m_Count);
//     auto dataSize = variable.m_ElementSize * numberElements;

//     // auto stepBlockID = g_strdup_printf("%lu_%lu", currStep, block);
//     auto uniqueID = g_strdup_printf("%d", entryID);
//     auto stringDataObject =
//         g_strdup_printf("%s_%s_%s", nameSpace.c_str(),
//         variable.m_Name.c_str(),
//                         objName.c_str());
//     // const char id = (char) entryID;

//     // auto dataObject = j_object_new(stringDataObject, stepBlockID);
//     auto dataObject = j_object_new(stringDataObject, uniqueID);

//     j_object_create(dataObject, batch);
//     j_object_write(dataObject, data, dataSize, 0, &bytesWritten, batch);
//     g_assert_true(j_batch_execute(batch) == true);

//     if (bytesWritten == dataSize)
//     {
//         // std::cout << "++ Julea Interaction Writer: Data written for:  "
//         // << stepBlockID << std::endl;
//     }
//     else
//     {
//         std::cout << "WARNING: only " << bytesWritten
//                   << " bytes written instead of " << dataSize << " bytes! "
//                   << std::endl;
//     }
//     g_free(stringDataObject);
//     j_object_unref(dataObject);
//     j_batch_unref(batch);
//     j_semantics_unref(semantics);

//     // std::cout << "++ Julea Interaction: PutVariableDataToJulea" <<
//     std::endl;
// }

#define declare_template_instantiation(T)                                      \
    template void JuleaDBInteractionWriter::PutVariableMetadataToJulea(        \
        core::Variable<T> &variable, const std::string nameSpace,              \
        const std::string varName, size_t currStep, size_t block);             \
    template void JuleaDBInteractionWriter::PutBlockMetadataToJulea(            \
        core::Variable<T> &variable, const std::string nameSpace,              \
        const std::string varName, size_t step, size_t block,                  \
        const typename core::Variable<T>::Info &blockInfo, T &blockMin,        \
        T &blockMax, T &blockMean, uint32_t &entryID);
ADIOS2_FOREACH_STDTYPE_1ARG(declare_template_instantiation)
#undef declare_template_instantiation

// #define declare_template_instantiation(T)                                      \
//     template void JuleaDBInteractionWriter::DAIDBPutVariableMetadataToJulea(                                \
//         Variable<T> &variable, const std::string nameSpace,                    \
//         const std::string varName, size_t currStep, size_t block);             \
//     template void JuleaDBInteractionWriter::DAIDBPutBlockMetadataToJulea(                                   \
//         Variable<T> &variable, const std::string nameSpace,                    \
//         const std::string varName, size_t step, size_t block,                  \
//         const typename Variable<T>::Info &blockInfo, T &blockMin, T &blockMax, \
//         T &blockMean, uint32_t &entryID);
// ADIOS2_FOREACH_STDTYPE_1ARG(declare_template_instantiation)
// #undef declare_template_instantiation

} // end namespace interop
} // end namespace adios2
