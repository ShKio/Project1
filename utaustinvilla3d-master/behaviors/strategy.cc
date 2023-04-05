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
    VecPosition Posball = worldModel->getBall();
    VecPosition Target[6] = {
            Posball + VecPosition(-0.5, 0, 0),                                          //控球位
            VecPosition(-15, Posball.getY() /10.0 * 1.05, 0),                           //守门位
            Posball + VecPosition(9, 4, 0),                                             //接应位
            Posball + VecPosition(9, -4, 0),                                            //接应位
            VecPosition((Posball.getX()-15)/2,Posball.getY()/2+5),                      //防守位
            VecPosition((Posball.getX()-15)/2,Posball.getY()/2-5),                      //防守位
            // VecPosition((Posball.getX()+15)/2+5* ::sqrt(1+(Posball.getX()+15/Posball.getY())),
            //             Posball.getY()/2-5*(Posball.getX()+15)/Posball.getY(),
            //             0),
            // VecPosition((Posball.getX()-15)/2+5* ::sqrt(1+(Posball.getX()+15/Posball.getY())),
            //             Posball.getY()/2+5*(Posball.getX()+15)/Posball.getY(),
            //             0)
    };

    //    5. 若点位超出了球场边界，请将点位移到球场内的合理对应位置中
    for(int i =2;i<=6;i++){
        if(Target[i].getX()>15){
            Target[i].setX(15);
        }
    }
    for(int i =2;i<=6;i++){
        if(Target[i].getX()<-15){
            Target[i].setX(-15);
        }
    }
    for(int i =2;i<=6;i++){
        if(Target[i].getY()>10){
            Target[i].setY(10);
        }
    }

    for(int i =2;i<=6;i++){
        if(Target[i].getY()<-10){
            Target[i].setY(-10);
        }
    }


    //创建二维数组储存
    vector <vector<double>> dis(6, vector<double>(6, 0));
    int BotForTarget[6] = {0};
    VecPosition Pos;
    //计算花费
    //4. 花费设置为：球员距跑位点的距离+是否摔倒(摔倒花费+1)，若球员所处位置无效，则花费设为最大值
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            int playerNum = WO_TEAMMATE1 + j;
            WorldObject *tem = worldModel->getWorldObject(playerNum);
            if (worldModel -> getUNum() == playerNum){
                Pos = me;//若是遍历到"我",则获取"我"的位置
                // if(worldModel->getf)
            }
            else {
                tem = worldModel->getWorldObject(WO_TEAMMATE1 + j);
                Pos = tem->pos;//若是队友则获取队友的位置
            }
            if(worldModel->getFallenTeammate(playerNum)){//球员距跑位点的距离+是否摔倒(摔倒花费+1)
                dis[i][j] = Target[i].getDistanceTo(Pos)+1;
            }
            else if (tem->validPosition){//若球员所处位置无效，则花费设为最大值
                dis[i][j]=10000;
            }
            else
                dis[i][j] = Target[i].getDistanceTo(Pos);//dis[i][j]保存第i个target到第j名球员的位置
        }
    }

    //分配任务
    for (int i = 0; i < 6; i++) {
        int robot = min_element(dis[i].begin(), dis[i].begin() + 6) - dis[i].begin();//找到花费最⼩的球员
        BotForTarget[i] = robot;//保存该球员编号

        for (int j = 0; j < 6; j++){
            dis[j][robot] = 10000;//不再考虑该球员，就将他的花费全设成最⼤
        }
    }

    //绘制函数
    //void drawText(string name, string text, double x, double y,RVSender::Color c=(RVSender::Color)uNum);
    
        worldModel->getRVSender()->drawPoint("Point", Target[0].getX(),Target[0].getY(), 1.0f, RVSender::MAGENTA);
        worldModel->getRVSender()->drawText("GOAL","KongQiu",Target[0].getX(), Target[0].getY(), RVSender::BLUE);
        worldModel->getRVSender()->drawPoint("Point", Target[1].getX(),Target[1].getY(), 1.0f, RVSender::MAGENTA);
        worldModel->getRVSender()->drawText("GOAL","ShouMen",Target[1].getX(), Target[1].getY(), RVSender::BLUE);
        worldModel->getRVSender()->drawPoint("Point", Target[2].getX(),Target[2].getY(), 1.0f, RVSender::MAGENTA);
        worldModel->getRVSender()->drawText("GOAL","JieYing1",Target[2].getX(), Target[2].getY(), RVSender::BLUE);
        worldModel->getRVSender()->drawPoint("Point", Target[3].getX(),Target[3].getY(), 1.0f, RVSender::MAGENTA);
        worldModel->getRVSender()->drawText("GOAL","JieYing2",Target[3].getX(), Target[3].getY(), RVSender::BLUE);
        worldModel->getRVSender()->drawPoint("Point", Target[4].getX(),Target[4].getY(), 1.0f, RVSender::MAGENTA);
        worldModel->getRVSender()->drawText("GOAL","FangShou1",Target[4].getX(), Target[4].getY(), RVSender::BLUE);
        worldModel->getRVSender()->drawPoint("Point", Target[5].getX(),Target[5].getY(), 1.0f, RVSender::MAGENTA);
        worldModel->getRVSender()->drawText("GOAL","FangShou2",Target[5].getX(), Target[5].getY(), RVSender::BLUE);
    

    
    for(int i=0;i<6;i++){
        if (worldModel->getUNum() == WO_TEAMMATE1 + BotForTarget[i]) {                           
            if(i==0){          //控球                                                                  
                if (me.getDistanceTo(Target[0]) > 1)                    
                    return goToTarget(collisionAvoidance(true, false, false, 1, .5, Target[0], true));
                else {        
                    //若球不在接应点附近且我在控球且我不在己方禁区内，则将球踢向离我较近的接应点处，
                    if (me.getX() > -15 + 3.9 && me.getY() > -10 + 0.9 && Posball.getDistanceTo(Target[2]) > 1 && Posball.getDistanceTo(Target[3]) > 1) {
                        if (me.getDistanceTo(Target[2]) < me.getDistanceTo(Target[3]))  //踢向最近的接应位
                            return kickBall(KICK_FORWARD, Target[2]);
                        else
                            return kickBall(KICK_FORWARD, Target[3]);
                    }

                        //若球不在接应点附近且我在控球且我在己方禁区内，则将球踢向离我最近的队友，
                    else if (worldModel->getMyPosition().getX() < -15 + 3.9 && worldModel->getMyPosition().getY() < -10 + 0.9 && Posball.getDistanceTo(Target[2]) > 1 && Posball.getDistanceTo(Target[3]) > 1) {
                        if (me.getDistanceTo(Target[2]) < me.getDistanceTo(Target[3]) && me.getDistanceTo(Target[2])<me.getDistanceTo(Target[1]))        //踢向最近的接应位
                            return kickBall(KICK_FORWARD, Target[2]);
                        else if(me.getDistanceTo(Target[2]) > me.getDistanceTo(Target[3])
                                && me.getDistanceTo(Target[2])>me.getDistanceTo(Target[1]))
                            return kickBall(KICK_FORWARD, Target[3]);
                        else
                            return kickBall(KICK_FORWARD,Target[1]);
                    }
                        //若球在接应点附近且我在控球，就将球踢向距离我较远的对方门柱。
                    else {
                        if (me.getY() > 0)
                            return kickBall(KICK_FORWARD, VecPosition(15, -HALF_GOAL_Y, 0));
                        else
                            return kickBall(KICK_FORWARD, VecPosition(15, HALF_GOAL_Y, 0));
                    }
                }
            
            }
            else{                                        //如果我不是离球最近的，那么我就跑去自己的点位，
            return goToTarget(collisionAvoidance(true, false, false, 1, .5,Target[i], true));
            }
        }
        
}
    
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

        double distanceToBall = temp.getDistanceTo(ball);
        if (distanceToBall < closestDistanceToBall) {
            playerClosestToBall = playerNum;
            closestDistanceToBall = distanceToBall;
        }
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


