#include "naobehavior.h"
#include "../rvdraw/rvdraw.h"

extern int agentBodyType;

/*
 * Real game beaming.
 * Filling params x y angle
 */
void NaoBehavior::beam( double& beamX, double& beamY, double& beamAngle ) {
    beamX = -HALF_FIELD_X + worldModel->getUNum();
    beamY = 0;
    beamAngle = 0;
}


SkillType NaoBehavior::selectSkill() {
    // My position and angle
    //cout << worldModel->getUNum() << ": " << worldModel->getMyPosition() << ",\t" << worldModel->getMyAngDeg() << "\n";

    // Position of the ball
    //cout << worldModel->getBall() << "\n";

    // Example usage of the roboviz drawing system and RVSender in rvdraw.cc.
    // Agents draw the position of where they think the ball is
    // Also see example in naobahevior.cc for drawing agent position and
    // orientation.
    /*
    worldModel->getRVSender()->clear(); // erases drawings from previous cycle
    worldModel->getRVSender()->drawPoint("ball", ball.getX(), ball.getY(), 10.0f, RVSender::MAGENTA);
    */

    // ### Demo Behaviors ###

    // Walk in different directions
    //return goToTargetRelative(VecPosition(1,0,0), 0); // Forward
    //return goToTargetRelative(VecPosition(-1,0,0), 0); // Backward
    //return goToTargetRelative(VecPosition(0,1,0), 0); // Left
    //return goToTargetRelative(VecPosition(0,-1,0), 0); // Right
    //return goToTargetRelative(VecPosition(1,1,0), 0); // Diagonal
    //return goToTargetRelative(VecPosition(0,1,0), 90); // Turn counter-clockwise
    //return goToTargetRelative(VecPdosition(0,-1,0), -90); // Turn clockwise
    //return goToTargetRelative(VecPosition(1,0,0), 15); // Circle

    // Walk to the ball
    //return goToTarget(ball);

    // Turn in place to face ball
    /*double distance, angle;
    getTargetDistanceAndAngle(ball, distance, angle);
    if (abs(angle) > 10) {
      return goToTargetRelative(VecPosition(), angle);
    } else {
      return SKILL_STAND;
    }*/

    // Walk to ball while always facing forward
    //return goToTargetRelative(worldModel->g2l(ball), -worldModel->getMyAngDeg());

    // Dribble ball toward opponent's goal
    //return kickBall(KICK_DRIBBLE, VecPosition(HALF_FIELD_X, 0, 0));

    // Kick ball toward opponent's goal
    //return kickBall(KICK_FORWARD, VecPosition(HALF_FIELD_X, 0, 0)); // Basic kick
    //return kickBall(KICK_IK, VecPosition(HALF_FIELD_X, 0, 0)); // IK kick

    // Just stand in place
    //return SKILL_STAND;

    // Demo behavior where players form a rotating circle and kick the ball
    // back and forth
    // return demoKickingCircle();
    return TurtleDefense();
}

SkillType NaoBehavior::TurtleDefense() {
    VecPosition ourgoal = VecPosition(-HALF_FIELD_X+1.5, 0, 0);
    VecPosition theirgoal = VecPosition(HALF_FIELD_X, 0, 0);
    bool danger = ourgoal.getDistanceTo(ball) < 12;
    bool extreme_danger = ourgoal.getDistanceTo(ball) < 6;
    // TODO for first four players, defend around goal.
    if (worldModel->getUNum()<5){
        VecPosition target = ourgoal;
        if (worldModel->getUNum()<3 && danger){
            target = ball;
        }
        if ( extreme_danger ){
            target = ball;
        }
        VecPosition mein_point = worldModel->getMyPosition();
        mein_point.setZ(0);
        if (mein_point.getDistanceTo(ball) < 3){
            return kickBall(KICK_FORWARD, theirgoal);
        }
        target = collisionAvoidance(true /*teammate*/, false/*opponent*/, true/*ball*/, 1/*proximity thresh*/, .5/*collision thresh*/, target, true/*keepDistance*/);
        return goToTarget(target);
    }

    // TODO for rest of the players:
    /*
    * Closest 4 player will rush to the ball and kick it
    * in the direction of the player who's furthest into the enemy half
    * MEANWHILE 3 other player will move into a fan area around the ball
    *
    * REPEAT, IF CLOSE ENOUGH, KICK TOWARD GOAL AT HIGHEST SPEED.
    */

    // FIND THE CLOSEST ROBOT TO THE BALL
    int playerClosestToBall = -1;
    double closestDistanceToBall = 10000;
    for(int i = WO_TEAMMATE5; i < WO_TEAMMATE5+NUM_AGENTS-4; ++i) {
        VecPosition temp;
        int playerNum = i - WO_TEAMMATE1 + 1;
        if (worldModel->getUNum() == playerNum) {
            // This is us
            temp = worldModel->getMyPosition();
        } else {
            WorldObject* teammate = worldModel->getWorldObject( i );
            if (teammate->validPosition) {
                temp = teammate->pos;
            } else {
                continue;
            }
        }
        temp.setZ(0);
        // Temp is set to be the x, y position of player closest to ball.

        double distanceToBall = temp.getDistanceTo(ball);
        if (distanceToBall < closestDistanceToBall) {
            playerClosestToBall = playerNum;
            closestDistanceToBall = distanceToBall;
        }
        // At the end of the loop we have playerClosestToBall and closestDistanceToBall
    }

    // FIND THE ROBOT CLOSEST TO ENEMY GOAL.
    int playerClosestToEnemy = -1;
    double closestDistanceToEnemy = 10000;
    for(int i = WO_TEAMMATE5; i < WO_TEAMMATE5+NUM_AGENTS-4; ++i) {
        VecPosition temp;
        int playerNum = i - WO_TEAMMATE1 + 1;
        if (worldModel->getUNum() == playerNum) {
            // This is us
            temp = worldModel->getMyPosition();
        } else {
            WorldObject* teammate = worldModel->getWorldObject( i );
            if (teammate->validPosition) {
                temp = teammate->pos;
            } else {
                continue;
            }
        }
        temp.setZ(0);
        // Temp is set to be the x, y position of player closest to ball.

        double distanceToBall = temp.getDistanceTo(theirgoal);
        if (distanceToBall < closestDistanceToBall) {
            playerClosestToEnemy = playerNum;
            closestDistanceToEnemy = distanceToBall;
        }
        // At the end of the loop we have playerClosestToBall and closestDistanceToBall
    }

    if (playerClosestToBall == worldModel->getUNum()) {
        if (playerClosestToBall == playerClosestToEnemy){
            if (worldModel->getMyPosition().getDistanceTo(theirgoal)< 10){
                return kickBall(KICK_FORWARD, theirgoal);
            } else {
                return kickBall(KICK_DRIBBLE, theirgoal);
            }
        } else {
            WorldObject* teammate = worldModel->getWorldObject(playerClosestToEnemy);
            VecPosition kickto = theirgoal;
            if (teammate->validPosition) {
                kickto = teammate->pos;
                kickto.setZ(0);
            }
            return kickBall(KICK_FORWARD, kickto);
        }
    } else {
        VecPosition target = ball;
        target = collisionAvoidance(true /*teammate*/, false/*opponent*/, true/*ball*/, 2/*proximity thresh*/, 1/*collision thresh*/, target, true/*keepDistance*/);
        return goToTarget(target);
    }
}


/*
 * Demo behavior where players form a rotating circle and kick the ball
 * back and forth
 */
SkillType NaoBehavior::demoKickingCircle() {
    // Parameters for circle
    VecPosition center = VecPosition(-HALF_FIELD_X/2.0, 0, 0);
    double circleRadius = 5.0;
    double rotateRate = 2.5;

    // Find closest player to ball
    int playerClosestToBall = -1;
    double closestDistanceToBall = 10000;
    for(int i = WO_TEAMMATE1; i < WO_TEAMMATE1+NUM_AGENTS; ++i) {
        VecPosition temp;
        int playerNum = i - WO_TEAMMATE1 + 1;
        if (worldModel->getUNum() == playerNum) {
            // This is us
            temp = worldModel->getMyPosition();
        } else {
            WorldObject* teammate = worldModel->getWorldObject( i );
            if (teammate->validPosition) {
                temp = teammate->pos;
            } else {
                continue;
            }
        }
        temp.setZ(0);
        // Temp is set to be the x, y position of player closest to ball.

        double distanceToBall = temp.getDistanceTo(ball);
        if (distanceToBall < closestDistanceToBall) {
            playerClosestToBall = playerNum;
            closestDistanceToBall = distanceToBall;
        }
        // At the end of the loop we have playerClosestToBall and closestDistanceToBall
    }

    if (playerClosestToBall == worldModel->getUNum()) {
        // Have closest player kick the ball toward the center
        return kickBall(KICK_FORWARD, center);
    } else {
        // Move to circle position around center and face the center
        VecPosition localCenter = worldModel->g2l(center);
        SIM::AngDeg localCenterAngle = atan2Deg(localCenter.getY(), localCenter.getX());

        // Our desired target position on the circle
        // Compute target based on uniform number, rotate rate, and time
        VecPosition target = center + VecPosition(circleRadius,0,0).rotateAboutZ(360.0/(NUM_AGENTS-1)*(worldModel->getUNum()-(worldModel->getUNum() > playerClosestToBall ? 1 : 0)) + worldModel->getTime()*rotateRate);

        // Adjust target to not be too close to teammates or the ball
        target = collisionAvoidance(true /*teammate*/, false/*opponent*/, true/*ball*/, 1/*proximity thresh*/, .5/*collision thresh*/, target, true/*keepDistance*/);

        if (me.getDistanceTo(target) < .25 && abs(localCenterAngle) <= 10) {
            // Close enough to desired position and orientation so just stand
            return SKILL_STAND;
        } else if (me.getDistanceTo(target) < .5) {
            // Close to desired position so start turning to face center
            return goToTargetRelative(worldModel->g2l(target), localCenterAngle);
        } else {
            // Move toward target location
            return goToTarget(target);
        }
    }
}
