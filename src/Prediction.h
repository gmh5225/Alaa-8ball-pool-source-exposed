#pragma once

#include "Vector.h"
#include "const.h"

#include <vector>

struct Collision;

struct Prediction
{
    Prediction()  {}
    ~Prediction() {}

    struct PredictionInfo
    {
        PredictionInfo()  {};
        ~PredictionInfo() {};

        void move(const vec_t& time);
        void findFirstCollision(PredictionInfo* info, Collision* collision, vec_t* time);
        bool isMovingOrSpinning();
        void calcVelocity();
        void calcVelocityPostCollision(const vec_t& angle);

        int                   state;
        int                   classification;
        bool                  onTable, originalOnTable;
        SIZE_T                index;
        Vector3D              spin;
        Vector2D              initialPos, predictedPosition, velocity;
        std::vector<Vector2D> positions;
    };

    bool init();
    void initAutoAim();
    void updateState(PredictionInfo* info, Collision* collision, int& state, int count);

    int            stateA, stateB;
    PredictionInfo infoA[maxBallsCount];
    PredictionInfo infoB[maxBallsCount];
private:
    void     handleCollision(Collision* collision, bool insert);
    void     handleBallBallCollision(Collision* collision);
    vec_t    getPowerAmount();
    void     setPowerAmount(vec_t value);
    Vector2D getSpinAmount();
};

extern Prediction* gPrediction;