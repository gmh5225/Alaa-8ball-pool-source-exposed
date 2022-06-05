#include "Prediction.h"
#include "GlobalVars.h"
#include "Offsets.h"
#include "Memory.h"

#include "Ball.h"
#include "Balls.h"
#include "TableInfo.h"
#include "GenericData.h"
#include "GameManager.h"
#include "Utils.h"
#include "GUI.h"

static Prediction prediction;
Prediction*       gPrediction = &prediction;

// Forward declarations of helper functions
vec_t scalePower(const vec_t& power);
vec_t calcAngle(const Vector2D& delta);
vec_t calcAngle(const Vector2D& source, const Vector2D& Destination);

static struct Collision
{
    Collision() { ZeroMemory(this, sizeof(*this)); }

    enum class Type : SIZE_T {
        Ball,
        Line,
        Point
    };

    bool                        valid;
    Type                        type;
    vec_t                       angle;
    Vector2D                    point;
    Prediction::PredictionInfo* infoA, * infoB, * firstHitBall;

} collisionA /* used for GUI prediction */, collisionB /* used for Aimbot prediction */;

void Prediction::PredictionInfo::calcVelocityPostCollision(const vec_t& angle)
{
    double v4; // r6
    double v5; // d1
    double v6; // d6
    double v7; // d7
    double v8; // d0
    double v9; // d3
    double v10; // d8
    double v11; // d9
    double v12; // d6
    double v13; // d7
    double v14; // d7
    double v15; // d14
    double v16; // d10
    double v17; // d14
    double v18; // d5
    double v19; // d4
    double v20; // d11
    double v21; // d5
    double v22; // d9
    double v24; // d10
    double v25; // d4
    double v26; // d5
    double v27; // d3
    double v28; // d8
    double v29; // d6

    v4 = -cos(angle);
    v5 = -sin(angle);
    v6 = this->velocity.y;
    v7 = this->velocity.x;
    v8 = this->spin.z;
    v9 = Ball::getRadius();
    v10 = v4 * v7 - v5 * v6;
    v11 = v10 - v8 * v9;
    v12 = v5 * v7 + v4 * v6;
    v13 = v10 - v8 * v9;
    if (v11 < 0.0)
        v13 = -v11;
    v14 = v13 / 2.5;
    v15 = v12;
    if (v12 < 0.0)
        v15 = -v12;
    v16 = 2.0 * 0.2 * v15;
    v17 = -1.0;
    if (v11 > 0.0)
        v17 = 1.0;
    v18 = this->spin.x;
    v19 = this->spin.y;
    v20 = v4 * v18;
    v21 = v5 * v18;
    if (v14 < v16)
        v16 = v14;
    v22 = v17 * v16;
    v24 = v5 * v19;
    v25 = v21 + v4 * v19;
    v26 = v20 - v24 - v12 * 0.54 / v9;
    v27 = 2.5 * v22 / v9;
    v28 = v10 - v22 / 2.5;
    v29 = -(v12 * 0.804);
    this->velocity.x = v5 * v29 + v4 * v28;
    this->velocity.y = v4 * v29 - v28 * v5;
    this->spin.x = v5 * v25 + v4 * v26;
    this->spin.y = v4 * v25 - v26 * v5;
    this->spin.z = v8 + v27;
}

void Prediction::PredictionInfo::move(const vec_t& time)
{
    if (this->velocity.x != vecZero) {
        this->predictedPosition.x += this->velocity.x * time;
        this->predictedPosition.y += this->velocity.y * time;
    }
}

void Prediction::PredictionInfo::calcVelocity()
{
    bool result;
    Vector2D tempVelocity;
    Vector3D tempSpin;
    double ballRadius; // d13
    double v11; // d14
    double v12; // d15
    double v14; // d2
    double v15; // d1
    double v17; // d3
    double v18; // d2
    double v19; // d3
    double v20; // d2
    double v21; // d12
    double v22; // d11
    double v23; // d0
    double v24; // d0

    result = this->isMovingOrSpinning();
    if (result) {
        tempSpin     = this->spin;
        tempVelocity = this->velocity;
        ballRadius  = Ball::getRadius();
        v11 = tempSpin.x * ballRadius - tempVelocity.y;
        v12 = -tempVelocity.x - tempSpin.y * ballRadius;
        v14 = sqrt(v12 * v12 + v11 * v11);
        v15 = v14 * 0.00145772594752187;
        if (v14 * 0.00145772594752187 > 1E-11) {
            v17 = gTimePerTick;
            if (v15 < gTimePerTick)
                v17 = v14 * 0.00145772594752187;
            v18 = 196.0 * v17 / v14;
            v19 = v12 * v18;
            v20 = v11 * v18;
            tempVelocity.y += v20;
            tempVelocity.x += v19;
            tempSpin.y += 2.5 * v19 / ballRadius;
            tempSpin.x -= 2.5 * v20 / ballRadius;
        }

        if (v15 < gTimePerTick) {
            v21 = (gTimePerTick - v15) * 10.878;
            v22 = 0.0;
            v23 = 1.0 - v21 / tempVelocity.length();
            if (v22 > v23)
                v23 = v22;
            tempVelocity.y *= v23;
            tempSpin.y = -(tempVelocity.x * v23) / ballRadius;
            tempSpin.x = tempVelocity.y / ballRadius;
            tempVelocity.x *=  v23;
        }

        v24 = 9.8 * gTimePerTick;
        if (tempSpin.z <= 0.0)
        {
            tempSpin.z += v24;
            if (0.0 < tempSpin.z)
                tempSpin.z = 0.0;
        }
        else
        {
            tempSpin.z -= v24;
            if (0.0 > tempSpin.z)
                tempSpin.z = 0.0;
        }

        this->velocity = tempVelocity;
        this->spin     = tempSpin;
    }
}

void Prediction::handleBallBallCollision(Collision* collision)
{
    double v4; // d12
    double v5; // d13
    double v6; // d8
    double v7; // d9
    double v8; // d10
    double v9; // d11
    double v10; // d0
    double v12; // d0
    double v13; // d1
    double v14; // d0
    double v15; // d2
    double v16; // d3
    double v17; // d4
    double v18; // d2
    double v19; // d0
    double v20; // d1

    Prediction::PredictionInfo* infoA = collision->infoA;
    Prediction::PredictionInfo* infoB = collision->infoB;

    v4 = infoA->predictedPosition.x - infoB->predictedPosition.x;
    v5 = infoA->predictedPosition.y - infoB->predictedPosition.y;
    v6 = infoB->velocity.x;
    v7 = infoB->velocity.y;
    v8 = infoA->velocity.x;
    v9 = infoA->velocity.y;
    v10 = sqrt(v4 * v4 + v5 * v5);
    v12 = 1.0 / v10;
    v13 = v5 * v12;
    v14 = v4 * v12;
    v15 = -(v14 * v8) - v9 * v13;
    v16 = v6 * v14 + v7 * v13;
    v17 = -(v14 * v15);
    v18 = -(v13 * v15);
    v19 = v14 * v16;
    v20 = v13 * v16;
    infoA->velocity.x = v19 - (v17 - v8);
    infoA->velocity.y = v20 - (v18 - v9);
    infoB->velocity.x = v17 - (v19 - v6);
    infoB->velocity.y = v18 - (v20 - v7);
}

void sub_1BC1FEC(double* a1, Prediction::PredictionInfo* a2, Prediction::PredictionInfo* a3, double* a4)
{
    double* v4; // r4
    double* v5; // r5
    double v6; // d0
    double v7; // d2
    double v8; // d1
    double v9; // d3
    double v10; // d9
    double v11; // d4
    double v12; // d8
    double v13; // d0
    double v14; // d0

    v4 = a1;
    v5 = a4;
    v6 = a3->predictedPosition.y - a2->predictedPosition.y;
    v7 = a3->velocity.y - a2->velocity.y;
    v8 = a3->predictedPosition.x - a2->predictedPosition.x;
    v9 = a3->velocity.x - a2->velocity.x;
    v10 = 2.0 * (v8 * v9 + v6 * v7);
    if (v10 >= 0.0
        || (v11 = Ball::getRadius() + Ball::getRadius(),
            v12 = v9 * v9 + v7 * v7,
            v13 = (v8 * v8 + v6 * v6 - v11 * v11) * (v12 * 4.0),
            v10 * v10 < v13))
    {
        goto LABEL;
    }
    v14 = (-v10 - sqrt(v10 * v10 - v13)) / (v12 * 2.0);
    if (v14 < 0.0 || (v14 - 1E-11 > *v5))
    {
    LABEL:
        *v4 = *a4;
        return;
    }

    *v4 = v14;
}

void Prediction::handleCollision(Collision* collision, bool insert)
{
    Vector2D delta;

    if (insert)
        collisionA.infoA->positions.push_back(collisionA.infoA->predictedPosition);
    switch (collision->type)
    {
    case Collision::Type::Ball:
        handleBallBallCollision(collision);
        if (insert)
            collisionA.infoB->positions.push_back(collisionA.infoB->predictedPosition);
        if (collision->firstHitBall == nullptr) 
            collision->firstHitBall = collision->infoB;
        break;
    case Collision::Type::Line:
        collision->infoA->calcVelocityPostCollision(collision->angle);
        break;
    default:
        delta.x = collision->point.y - collision->infoA->predictedPosition.y;
        delta.y = -(collision->point.x - collision->infoA->predictedPosition.x);
        collision->angle = -calcAngle(delta);
        collision->infoA->calcVelocityPostCollision(collision->angle);
        break;
    }
}

void sub_1BC216C(double* pTime_1, Prediction::PredictionInfo* info, const Vector2D& a3, const Vector2D& a4, double* a5)
{
    double* pTime; // r8
    Prediction::PredictionInfo* v6; // r6
    double v10; // d11
    double v11; // d8
    double v12; // d12
    double v13; // d13
    double v14; // d9
    double v15; // d10
    double v16; // r0
    double v17; // d3
    double v19; // d6
    double v20; // d1
    double v21; // d5
    double v22; // d6
    double v23; // d5
    double v24; // d7
    double time; // d3


    if (info->velocity.isNull())
        goto LABEL_11;

    pTime = pTime_1;
    v6 = info;
    v10 = a4.y - a3.y;
    v11 = a4.x - a3.x;
    v12 = v6->predictedPosition.x;
    v13 = v6->predictedPosition.y;
    v14 = v6->velocity.x;
    v15 = v6->velocity.y;
    v16 = sqrt(v11 * v11 + v10 * v10);
    v17 = v10 * v14 - v11 * v15;
    if (v17 == 0.0) {
    LABEL_11:
        *pTime_1 = *a5;
        return;
    }
    v19 = Ball::getRadius();
    v20 = 1.0 / v16;
    v21 = v19 * (v11 * (1.0 / v16));
    v22 = v12 - a3.x - v10 * (1.0 / v16) * v19;
    v23 = v13 - a3.y + v21;
    v24 = (v22 * -v15 - v23 * -v14) / v17;
    if (v24 <= 0.0 || v24 >= 1.0)
    {
        goto LABEL_11;
    }
    time = (v11 * v23 - v10 * v22) / v17;
    if (time <= 0.0 || (time - 1E-11 > *a5))
    {
        goto LABEL_11;
    }
    if (v14 * (v10 * v20) + v15 * -(v11 * v20) > 0.0)
    {
        goto LABEL_11;
    }

    *pTime = time;
}

void sub_1BC2370(double* a1, Prediction::PredictionInfo* a2, const Vector2D& a3, double* a4)
{
    double* v4; // r4
    double* v5; // r5
    double v6; // d0
    double v7; // d1
    double v8; // d2
    double v9; // d3
    double v10; // d9
    double v11; // d8
    double v12; // d0
    double v14; // d0
    constexpr auto v13 = (Ball::getRadius() * Ball::getRadius());

    v4 = a1;
    v5 = a4;
    v6 = a2->velocity.x;
    v7 = a2->velocity.y;
    v8 = a3.y - a2->predictedPosition.y;
    v9 = a3.x - a2->predictedPosition.x;
    v10 = -(v6 * 2.0 * v9) - (v7 * 2.0 * v8);
    if (v10 >= 0.0)
    {
        goto LABEL_8;
    }
    v11 = v6 * v6 + v7 * v7;
    v12 = v9 * v9 + v8 * v8;
    if (((v12 - v10 * v10) / (v11 * 4.0)) >= v13)
    {
        goto LABEL_8;
    }
    v14 = (-v10 - sqrt(v10 * v10 - v11 * 4.0 * (v12 - v13))) / (v11 * 2.0);
    if ((v14 < 0.0) || (v14 - 1E-11 > *v5))
    {
    LABEL_8:
        *v4 = *a4;
        return;
    }

    *v4 = v14;
}

void Prediction::PredictionInfo::findFirstCollision(PredictionInfo* information, Collision* collision, vec_t* time)
{
    int             state;
    vec_t           angle, tempTime, smallestTime;
    Vector2D        tempPos, tempVelocity;
    const Vector2D* point, * nextPoint;
    SIZE_T          index, nextIndex, count;
    PredictionInfo* info;

    constexpr const vec_t ballRadius       = Ball::getRadius();
    constexpr const vec_t pocketRadius     = TableInfo::getPocketRadius();
    const auto*           tableShape       = &TableInfo::getTableShape();
    const auto*           pocketsPositions = &TableInfo::getPocketsPositions();

    state = this->state;
    smallestTime = *time;
    if (state == 1) {
        count = gBalls->getCount();
        index = this->index + 1;
        if (this->classification == 8)
            index = 1;
        if (index < count) {
            do
            {
                info = &information[index];
                if (info->onTable == true) {
                    sub_1BC1FEC(&tempTime, this, info, &smallestTime);
                    if (tempTime < smallestTime) {
                        smallestTime = tempTime;
                        collision->valid = true;
                        collision->infoA = this;
                        collision->type  = Collision::Type::Ball;
                        collision->infoB = info;
                    }
                }

                ++index;
            } while (count != index);
        }
    }

    tempPos      = this->predictedPosition;
    tempVelocity = this->velocity;
    vec_t v29 = tempPos.y + tempVelocity.y * smallestTime;
    vec_t v30 = tempPos.y;
    if (tempVelocity.y > 0.0)
        v30 = tempPos.y + tempVelocity.y * smallestTime;
    vec_t v31 = -127.0;
    vec_t v32 = tempPos.x + tempVelocity.x * smallestTime;
    if (tempVelocity.x > 0.0)
        v32 = tempPos.x;
    vec_t v33 = -63.5;
    vec_t v34 = 127.0;
    if (v32 >= ballRadius + v31) {
        vec_t v35 = tempPos.x;
        if (tempVelocity.x > 0.0)
            v35 = tempPos.x + tempVelocity.x * smallestTime;
        if (tempVelocity.y > 0.0)
            v29 = tempPos.y;
        if (v29 >= ballRadius + v33 && v35 <= v31 + 254.0 - ballRadius && v30 <= v33 + v34 - ballRadius)
            goto LABEL_61;
    }

    if (state != 1 || this->classification == 8) {
        if (state == 2) {
            vec_t v72 = 1.5 * smallestTime;
            tempVelocity.x = tempVelocity.x + (v31 + 254.0 * 0.5 - tempPos.x) * v72;
            tempVelocity.y = tempVelocity.y + v72 * (v33 + 0.5 * v34 - tempPos.y);
            this->velocity.x = tempVelocity.x;
            this->velocity.y = tempVelocity.y;
        }
    }
    else
    {
        for (const auto& pocketPosition : *pocketsPositions) {
            vec_t v41 = ballRadius * ballRadius;
            vec_t v43 = pocketPosition.y - tempPos.y;
            vec_t v45 = pocketPosition.x - tempPos.x;
            vec_t v47 = v45 * v45 + v43 * v43;
            if (v47 < pocketRadius * pocketRadius) {
                vec_t v49 = 120.0 * smallestTime;
                tempVelocity.x = tempVelocity.x + v45 * v49;
                tempVelocity.y = tempVelocity.y + v43 * v49;
                this->velocity.x = tempVelocity.x;
                this->velocity.y = tempVelocity.y;
                if (v47 < v41) {
                    this->state = 2;
                    break;
                }
            }
        }
    }

    index = 0UL;
    count = tableShape->size();
    if (index < count) {
        do
        {
            nextIndex = index + 1;
            point     = &tableShape->at(index);
            nextPoint = &tableShape->at(nextIndex % count);
            sub_1BC216C(&tempTime, this, *point, *nextPoint, &smallestTime);
            if (tempTime < smallestTime) {
                angle = calcAngle(*nextPoint, *point);
                collision->valid = true;
                collision->infoA = this;
                collision->type  = Collision::Type::Line;
                collision->angle = -angle;
                smallestTime     = tempTime;
            }

            sub_1BC2370(&tempTime, this, *point, &smallestTime);
            if (tempTime < smallestTime) {
                collision->valid = true;
                collision->infoA = this;
                collision->point = *point;
                collision->type  = Collision::Type::Point;
                smallestTime    = tempTime;
            }

        } while (++index < count);
    }

LABEL_61:
    if (state == 2) {
        this->state = 3;
        this->onTable = false;
        this->velocity.nullify();
        this->spin.nullify();
    }

    *time = smallestTime;
}

/* I need more work, please fix me  */
/* nominated pocket check!          */
/* isInCushionMode 50               */
/* any other table rules            */
void Prediction::updateState(PredictionInfo* information, Collision* collision, int& state, int count)
{
    int             myClassification, index;
    bool            newState, result;
    PredictionInfo* predictionInfo;

    state = false;
    if (collision->firstHitBall == nullptr)
        return;

    if (information[0].onTable == false)
        return;

    result           = false;
    myClassification = GameManager::playerClassification();
    if (GameManager::is9BallGame()) {
        state = (collision->firstHitBall->classification == myClassification);
        if (state == false)
            return;

        if (gGlobalVars->features.nineBallGameRule == 0) {
            for (index = 1; index < count; index++) {
                predictionInfo = &information[index];
                if (predictionInfo->originalOnTable != predictionInfo->onTable)
                    return;
            }

            state = false;
        }
        else
        {
            state = (information[9].originalOnTable != information[9].onTable);
        }

        return;
    }
    
    for (index = 1; index < count; index++) {
        predictionInfo = &information[index];
        if (predictionInfo->classification == myClassification || myClassification == -1) {
            if (predictionInfo->originalOnTable != predictionInfo->onTable) {
                if (index == 8) {
                    if (predictionInfo->classification == myClassification) {
                        result = true;
                    }
                    else
                    {
                        result = false;
                        break;
                    }
                }
                else
                {
                    result = true;
                }
            }
        }
    }

    if (myClassification == -1 && collision->firstHitBall->index != 8)
        newState = true;
    else if (collision->firstHitBall->classification == myClassification)
        newState = true;
    else
        newState = false;
        
    state = (result && newState);
}

bool Prediction::init()
{
    int             i, count;
    Ball*           ball;
    bool            result;
    vec_t           angle, angleCos, angleSin, powerAmount, time, time2, v29;
    Vector2D        spinAmount;
    GenericData*    genericData;
    PredictionInfo* predictionInfo;
    PredictionInfo* cueBallInfo = &this->infoA[0];

    if (gBalls->init() && GenericData::get(&genericData)) {
        count       = gBalls->getCount();
        spinAmount  = getSpinAmount();
        powerAmount = getPowerAmount();
        powerAmount = scalePower(powerAmount);
        angle       = genericData->getAimAngle();
        for (i = 0UL; i < ARRAYSIZE(this->infoA); i++) {
            predictionInfo = &this->infoA[i];
            if (predictionInfo->positions.size() > 0UL)
                predictionInfo->positions.clear();
            if (i < count)
                predictionInfo->positions.reserve(20);
        }

        for (i = 0UL; i < count; i++) {
            ball = gBalls->getBall(i);
            predictionInfo = &this->infoA[i];
            predictionInfo->index = i;
            predictionInfo->onTable = ball->isOnTable();
            predictionInfo->originalOnTable = predictionInfo->onTable;
            predictionInfo->state = predictionInfo->onTable ? 1 : 3;
            predictionInfo->classification = ball->classification();
            predictionInfo->initialPos = ball->getPosition();
            predictionInfo->predictedPosition = predictionInfo->initialPos;
            predictionInfo->velocity.nullify();
            predictionInfo->spin.nullify();

            predictionInfo->positions.push_back(predictionInfo->initialPos);
        }

        angleCos                = cos(angle);
        angleSin                = sin(angle);
        v29                     = -(spinAmount.y * powerAmount) / Ball::getRadius();
        cueBallInfo->velocity.x = angleCos * powerAmount;
        cueBallInfo->velocity.y = angleSin * powerAmount;
        cueBallInfo->spin.x     = -(angleSin * v29);
        cueBallInfo->spin.y     = (angleCos * v29);
        cueBallInfo->spin.z     = spinAmount.x * powerAmount / Ball::getRadius();
        collisionA.firstHitBall = nullptr;
        do
        {
            time = gTimePerTick;
            do
            {
                time2 = time;
                collisionA.valid = false;
                for (i = 0UL; i < count; i++) {
                    predictionInfo = &this->infoA[i];
                    if (predictionInfo->onTable)
                        predictionInfo->findFirstCollision(this->infoA, &collisionA, &time2);
                }

                for (i = 0UL; i < count; i++) {
                    predictionInfo = &this->infoA[i];
                    result = predictionInfo->isMovingOrSpinning();
                    if (result && predictionInfo->onTable) {
                        predictionInfo->move(time2);
                    }
                }

                if (collisionA.valid) {
                    this->handleCollision(&collisionA, true);
                }

                time = time - time2;
            } while (time > 1E-11);

            for (i = 0UL; i < count; i++) {
                predictionInfo = &this->infoA[i];
                if (predictionInfo->onTable) {
                    predictionInfo->calcVelocity();
                }
            }

            result = false;
            for (i = 0UL; i < count; i++) {
                predictionInfo = &this->infoA[i];
                result = predictionInfo->onTable && predictionInfo->isMovingOrSpinning();
                if (result)
                    break;
            }

        } while (result == true);

        updateState(this->infoA, &collisionA, this->stateA, count);
        for (i = 0UL; i < count; i++) {
            predictionInfo = &this->infoA[i];
            predictionInfo->positions.push_back(predictionInfo->predictedPosition);
        }

        return true;
    }

    return false;
}

void Prediction::initAutoAim()
{
    int             i, count, clickPos[2], tablePos[4], temp;
    Ball*           ball;
    bool            result;
    vec_t           angle, actualAngle, angleCos, angleSin, power, actuallPower, scaledPower, time, time2, v29;
    Vector2D        spinAmount;
    GenericData*    genericData;
    PredictionInfo* predictionInfo;
    PredictionInfo* cueBallInfo = &this->infoB[0];
    constexpr vec_t maximumPower = 1.0; // not really exact

    bool isGoldenShot = GameManager::getGameMode() == 16;
    if (isGoldenShot) 
        return;
    if (gBalls->init() && GenericData::get(&genericData)) {
        count        = gBalls->getCount();
        spinAmount   = getSpinAmount();
        actualAngle  = genericData->getAimAngle();
        actuallPower = this->getPowerAmount();
        if (gGlobalVars->features.automatic == 2) {
            if (gGlobalVars->features.powerMode == 0) {
                temp = getRandomInt(gGlobalVars->features.randPowerBounds[0], gGlobalVars->features.randPowerBounds[1]);
                power = static_cast<double>(temp);
            }
            else
            {
                power = gGlobalVars->features.autoPlayMaxPower;
            }

            power *= 0.01;
        }
        else
        {
            power = maximumPower;
        }

    powerLabel:
        angle        = vecZero;
        scaledPower  = scalePower(power);
    angleLabel:
        for (i = 0UL; i < count; i++) {
            ball = gBalls->getBall(i);
            predictionInfo = &this->infoB[i];
            predictionInfo->index = i;
            predictionInfo->onTable = ball->isOnTable();
            predictionInfo->originalOnTable = predictionInfo->onTable;
            predictionInfo->state = predictionInfo->onTable ? 1 : 3;
            predictionInfo->classification = ball->classification();
            predictionInfo->initialPos = ball->getPosition();
            predictionInfo->predictedPosition = predictionInfo->initialPos;
            predictionInfo->velocity.nullify();
            predictionInfo->spin.nullify();
        }

        angleCos                = cos(angle);
        angleSin                = sin(angle);
        v29                     = -(spinAmount.y * scaledPower) / Ball::getRadius();
        cueBallInfo->velocity.x = angleCos * scaledPower;
        cueBallInfo->velocity.y = angleSin * scaledPower;
        cueBallInfo->spin.x     = -(angleSin * v29);
        cueBallInfo->spin.y     = (angleCos * v29);
        cueBallInfo->spin.z     = spinAmount.x * scaledPower / Ball::getRadius();
        collisionB.firstHitBall = nullptr;
        do
        {
            time = gTimePerTick;
            do
            {
                time2 = time;
                collisionB.valid = false;
                for (i = 0UL; i < count; i++) {
                    predictionInfo = &this->infoB[i];
                    if (predictionInfo->onTable)
                        predictionInfo->findFirstCollision(this->infoB, &collisionB, &time2);
                }

                for (i = 0UL; i < count; i++) {
                    predictionInfo = &this->infoB[i];
                    result = predictionInfo->isMovingOrSpinning();
                    if (result && predictionInfo->onTable) {
                        predictionInfo->move(time2);
                    }
                }

                if (collisionB.valid) {
                    this->handleCollision(&collisionB, false);
                }

                time = time - time2;
            } while (time > 1E-11);

            for (i = 0UL; i < count; i++) {
                predictionInfo = &this->infoB[i];
                if (predictionInfo->onTable) {
                    predictionInfo->calcVelocity();
                }
            }

            result = false;
            for (i = 0UL; i < count; i++) {
                predictionInfo = &this->infoB[i];
                result = predictionInfo->onTable && predictionInfo->isMovingOrSpinning();
                if (result)
                    break;
            }

        } while (result == true);

        updateState(this->infoB, &collisionB, this->stateB, count);
        if ((angle + gTimePerTick) <= maxAngle && this->stateB == false) {
            angle += gTimePerTick;
            goto angleLabel;
        }

        if (gGlobalVars->features.automatic == 2) {
            if ((power - gTimePerTick) >= gTimePerTick && this->stateB == false) {
                power -= gTimePerTick;
                goto powerLabel;
            }
        }

        if (this->stateB) {
            switch (gGlobalVars->features.automatic) {
            case 1:
                if (angle != actualAngle) {
                    genericData->setAimAngle(angle);
                }
                break;
            case 2:
                if (angle != actualAngle) {
                    genericData->setAimAngle(angle);
                }

                this->setPowerAmount(power);
                tablePos[0] = static_cast<int>(TableInfo::Position::left());
                tablePos[1] = static_cast<int>(TableInfo::Position::top());
                tablePos[2] = static_cast<int>(TableInfo::Position::right());
                tablePos[3] = static_cast<int>(TableInfo::Position::bottom());
                clickPos[0] = getRandomInt(tablePos[0], tablePos[2]);
                clickPos[1] = getRandomInt(tablePos[1], tablePos[3]);
                SendMessage(AEngineRenderWindow::window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(clickPos[0], clickPos[1]));
                SendMessage(AEngineRenderWindow::window, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(clickPos[0], clickPos[1]));
                break;
            }
        }
    }
}

static vec_t calcAngle(const Vector2D& source, const Vector2D& Destination)
{
    vec_t    angle;
    Vector2D delta;

    delta.x = source.x - Destination.x;
    delta.y = source.y - Destination.y;
    angle = calcAngle(delta);

    return angle;
}

static vec_t calcAngle(const Vector2D& delta)
{
    return atan2(delta.y, delta.x) + vecPI;
}

vec_t Prediction::getPowerAmount()
{
    vec_t  power;
    SIZE_T gameManager;

    constexpr vec_t defaultPowerAmount = 1;

    gameManager = gGlobalVars->memory->read<SIZE_T>(gGlobalVars->sharedGameManager + 0x148UL);
    if (gameManager == 0ULL) goto failure;
    gameManager = gGlobalVars->memory->read<SIZE_T>(gameManager + 0x18CUL);
    if (gameManager == 0ULL) goto failure;

    power = gGlobalVars->memory->read<vec_t>(gameManager + 0x280UL);
    if (power == 0.0) {
    failure:
        power = defaultPowerAmount;
    }

    return power;
}

vec_t scalePower(const vec_t& power)
{
    vec_t maxPower, result;

    maxPower = gGlobalVars->memory->read<vec_t>(gGlobalVars->gameModuleBase + gOffsets::CUE_PROPERTIES_MAX_POWER);
    result = 1.0 - sqrt(1.0 - power * maxPower / maxPower);
    result *= maxPower;

    return result;
}

void Prediction::setPowerAmount(vec_t value)
{
    SIZE_T buffer;
    //vec_t  powerAmount;

    buffer = gGlobalVars->memory->read<SIZE_T>(gGlobalVars->sharedGameManager + 0x148UL);
    if (buffer == 0ULL)
        return;
    buffer = gGlobalVars->memory->read<SIZE_T>(buffer + 0x18CUL);
    if (buffer == 0ULL)
        return;

    //powerAmount = gGlobalVars->memory->read<vec_t>(buffer + 0x280UL);
    //if (powerAmount != value)
    gGlobalVars->memory->write(buffer + 0x280UL, value);
}

Vector2D Prediction::getSpinAmount()
{
    Vector2D spin;
    vec_t    maxSpin;
    SIZE_T   gameManager;

    constexpr vec_t defaultSpinAmount = vecZero;

    gameManager = gGlobalVars->memory->read<SIZE_T>(gGlobalVars->sharedGameManager + 0x2D8UL);
    if (gameManager == 0ULL) goto failure;
    gameManager = gGlobalVars->memory->read<SIZE_T>(gameManager + 0x1FCUL);
    if (gameManager == 0ULL) goto failure;
    gameManager = gGlobalVars->memory->read<SIZE_T>(gameManager + 0x2E0UL);
    if (gameManager == 0ULL) goto failure;

    gGlobalVars->memory->read(gameManager + 0x298UL, &spin, sizeof(spin));
    goto success;

failure:
    spin.x = spin.y = defaultSpinAmount;

success:
    maxSpin = gGlobalVars->memory->read<vec_t>(gGlobalVars->gameModuleBase + gOffsets::CUE_PROPERTIES_SPIN);
    spin.x *= maxSpin;
    spin.y *= maxSpin;

    return spin;
}

bool Prediction::PredictionInfo::isMovingOrSpinning()
{
    //return ((this->velocity.isNull() && this->spin.isNull()) == false);
    return ((this->velocity.isNull() && this->spin.x == vecZero && this->spin.y == vecZero) == false); // use this for better performance
}