#include "TableInfo.h"
#include "GameManager.h"
#include "GlobalVars.h"
#include "Memory.h"

#include "OneTimeAssignment.h"

static Vector2D                                   size{};
static Vector4D                                   position{};
static OneTimeAssignment<std::vector<Vector2D>>   tableShape{};
static OneTimeAssignment<std::array<Vector2D, 6>> pocketsPositions{};

void TableInfo::Position::set(vec_t x, vec_t y, vec_t z, vec_t w)
{
    position.x = x;
    position.y = y;
    position.z = z;
    position.w = w;
}

void TableInfo::setSize(vec_t x, vec_t y)
{
    size.x = x;
    size.y = y;
}

vec_t TableInfo::Position::left()
{
    return position.x;
}

vec_t TableInfo::Position::top()
{
    return position.y;
}

vec_t TableInfo::Position::right()
{
    return position.z;
}

vec_t TableInfo::Position::bottom()
{
    return position.w;
}

vec_t TableInfo::getWidth()
{
    return size.x;
}

vec_t TableInfo::getHeight()
{
    return size.y;
}

const std::array<Vector2D, 6>& TableInfo::getPocketsPositions()
{
    PVOID  buffer;
    SIZE_T table, tableProperties, pockets, bufferSize;

    if (pocketsPositions.canAssign()) {
        table           = GameManager::getTable();
        tableProperties = gGlobalVars->memory->read<SIZE_T>(table + 0x284UL);
        if (tableProperties) {
            pockets = gGlobalVars->memory->read<SIZE_T>(tableProperties + 0x34UL);
            if (pockets) {
                bufferSize = sizeof(Vector2D) * maxPocketCount;
                buffer = malloc(bufferSize);
                if (buffer) {
                    if (gGlobalVars->memory->read(pockets, buffer, bufferSize)) {
                        memcpy(&pocketsPositions.data[0].x, buffer, bufferSize);
                        pocketsPositions.postAssignment();
                    }

                    free(buffer);
                }
            }
        }
    }

    return pocketsPositions.get();
}

const std::vector<Vector2D>& TableInfo::getTableShape()
{
    PVOID  buffer;
    SIZE_T table, vecStartAddr, vecEndAddr, vecSize;

    if (tableShape.canAssign()) {
        table = GameManager::getTable();
        vecStartAddr = gGlobalVars->memory->read<SIZE_T>(table + 0x2A4UL);
        vecEndAddr   = gGlobalVars->memory->read<SIZE_T>(table + 0x2A4UL + 4UL);
        vecSize      = vecEndAddr - vecStartAddr;
        if (vecStartAddr && vecEndAddr) {
            buffer = malloc(vecSize);
            if (buffer) {
                if (gGlobalVars->memory->read(vecStartAddr, buffer, vecSize)) {
                    tableShape.data.reserve(vecSize >> 4);
                    tableShape.data.resize(vecSize >> 4);
                    memcpy(&tableShape.data[0].x, buffer, vecSize);
                    tableShape.postAssignment();
                }

                free(buffer);
            }
        }
    }

    return tableShape.get();
}