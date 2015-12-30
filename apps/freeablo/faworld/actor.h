#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"

#include "../farender/renderer.h"
#include "../fasavegame/savegame.h"
#include <boost/format.hpp>
#include <misc/misc.h>
#include <map>

namespace Engine
{
    class NetManager;
}

namespace FAWorld
{
    class ActorStats;
    class World;

    namespace AnimState
    {
        enum AnimState
        {
            walk,
            idle,
            attack,
            dead,
            hit
        };
    }    
    class Actor : public NetObject
    {
        STATIC_NET_OBJECT_SET_CLASS_ID(0)

        public:
            Actor(const std::string& walkAnimPath="",
                  const std::string& idleAnimPath="",
                  const Position& pos = Position(0,0),
                  const std::string& dieAnimPath="",
                  ActorStats* stats=nullptr);

            void update(bool noclip, size_t ticksPassed);
            void setStats(ActorStats* stats);
            virtual ~Actor();
            virtual std::string getDieWav(){return "";}
            virtual std::string getHitWav(){return "";}
            virtual void setSpriteClass(std::string className){UNUSED_PARAM(className);}
            virtual void takeDamage(double amount);
            virtual int32_t getCurrentHP();
            bool mAnimPlaying = false;
            bool isAttacking = false;
            virtual FARender::FASpriteGroup* getCurrentAnim();
            void setAnimation(AnimState::AnimState state, bool reset=false);
            void setWalkAnimation(const std::string path);
            void setIdleAnimation(const std::string path);            
            AnimState::AnimState getAnimState();

            size_t getId()
            {
                return mId;
            }

            virtual void setLevel(GameLevel* level);
            GameLevel* getLevel();

            virtual bool attack(Actor * enemy)
            {
                UNUSED_PARAM(enemy);
                return false;
            }
            Position mPos;            
        //private: //TODO: fix this
            FARender::FASpriteGroup* mWalkAnim = FARender::getDefaultSprite();
            FARender::FASpriteGroup* mIdleAnim = FARender::getDefaultSprite();
            FARender::FASpriteGroup* mDieAnim = FARender::getDefaultSprite();
            FARender::FASpriteGroup* mAttackAnim = FARender::getDefaultSprite();
            FARender::FASpriteGroup* mHitAnim = FARender::getDefaultSprite();
        
            size_t mFrame;
            virtual void die();
            std::pair<size_t, size_t> mDestination;
            std::pair<size_t, size_t>& destination()
            {
                return mDestination;
            }

            bool isDead() const;
            bool isEnemy() const;
            std::map<AnimState::AnimState, size_t> mAnimTimeMap;
            ActorStats * mStats=nullptr;
            virtual size_t getWriteSize();
            virtual bool writeTo(ENetPacket *packet, size_t& position);
            virtual bool readFrom(ENetPacket *packet, size_t& position);

        protected:
            GameLevel* mLevel = NULL;

            bool mIsDead = false;
            friend class boost::serialization::access;

            template<class Archive>
            void save(Archive & ar, const unsigned int version) const
            {
                UNUSED_PARAM(version);

                ar & this->mPos;
                ar & this->mFrame;
                ar & this->mAnimState;
                ar & this->mDestination;
            }

            template<class Archive>
            void load(Archive & ar, const unsigned int version)
            {
                UNUSED_PARAM(version);

                ar & this->mPos;
                ar & this->mFrame;
                ar & this->mAnimState;
                ar & this->mDestination;
            }

            bool canIAttack(Actor * actor);

            BOOST_SERIALIZATION_SPLIT_MEMBER()
            AnimState::AnimState mAnimState;
            bool mIsEnemy;

        private:
            size_t mId;
            friend class Engine::NetManager;
    };
}

#endif
