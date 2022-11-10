#pragma once
#ifndef NAMESPACES
#define NAMESPACES
#endif

#include "MathLib.h"
#include "Color.h"

#include <functional>
#include <vector>
#include <unordered_map>

namespace junebug
{
// Helper macro, see below
#define __REGISTER_ACTOR__(T) JGame::mActorConstructors[#T] = &PureActor::__createInstance__<T>;
// A macro to register an arbitrary number of custom Actor classes for serialization
// This allows the engine to do some basic reflection on the classes
// (basically, to be able to create them from a string, like from a JSON Scene file)
// You'll want to invoke this macro before you start loading scenes
// Ideally, put it in JGame::LoadData() so the mappings are set before any default scenes are loaded
#define JB_REGISTER_ACTORS(...) MAP(__REGISTER_ACTOR__, __VA_ARGS__)

    /// @brief Possible Actor states
    enum class ActorState
    {
        Started,
        Active,
        Paused,
        Destroy
    };

    /// @brief A PureActor is the base class for all actors. It contains no private data.
    class PureActor
    {
    public:
        // Default constructor
        PureActor();
        // Empty Vec2 constructor for factory convenience. This constructor is not intended to be used.
        PureActor(Vec2<float> pos);
        // Default destructor
        virtual ~PureActor();

        // ToString
        virtual void ToString(std::ostream &out) const
        {
            out << "Depth(" << mDepth << ")";
        }
        // Printable
        [[nodiscard]] friend std::ostream &operator<<(std::ostream &os, const PureActor &v)
        {
            v.ToString(os);
            return os;
        }

        // Get the actor's state
        ActorState GetState() const { return mState; }
        // Set the actor's state
        void SetState(ActorState state) { mState = state; }
        // Mark the actor as paused. This will prevent it from being updated.
        void Pause() { mState = ActorState::Paused; }
        // Mark the actor as active (default). This will allow it to be updated
        void Unpause() { mState = ActorState::Active; }
        // Mark the actor as destroyed. This will remove it from the game.
        void Destroy() { mState = ActorState::Destroy; }
        // Set whether the actor should persist between scenes
        void SetPersistent(bool persistent) { mPersistent = persistent; }

        // Update the actor in the game loop
        /// @param dt The time since the last update
        void Update(float dt);

        // Vector of attached components
        std::vector<class Component *> mComponents;
        // Returns component of type T, or null if doesn't exist
        template <typename T>
        T *GetComponent() const
        {
            for (auto c : mComponents)
            {
                T *t = dynamic_cast<T *>(c);
                if (t != nullptr)
                {
                    return t;
                }
            }

            return nullptr;
        }

        // Check if the actor persists between scenes
        /// @return true if the actor persists between scenes, false otherwise
        inline bool IsPersistent() const { return mPersistent; }

        // Internal function to create an instance of a class
        template <typename T>
        static PureActor *__createInstance__(Vec2<int> pos) { return new T(pos); };

        // Set the actor's depth
        void SetDepth(int newDepth) { mDepth = newDepth; };

    protected:
        friend class JGame;
        // User-defined callback
        virtual void OnUpdate(float dt){};
        // User-defined callback
        virtual void OnFirstUpdate(float dt){};

        // Actor state
        ActorState mState = ActorState::Started;

        // Actor persistence
        bool mPersistent = false;

        //Actor depth
        int mDepth = 0;
    };

    /// @brief A VisualActor is an actor that has a visual representation, including a texture, position, rotation, scale, and color.
    class VisualActor : public PureActor
    {
    public:
        // Position constructor
        VisualActor(Vec2<float> pos);
        VisualActor(Vec2<int> pos);
        // Position and image constructor
        VisualActor(Vec2<float> pos, std::string imagePath, int drawOrder = 100);
        VisualActor(Vec2<int> pos, std::string imagePath, int drawOrder = 100);

        // ToString
        virtual void ToString(std::ostream &out) const override
        {
            out << "Pos" << mPosition << " Scale" << mScale << " ";
            PureActor::ToString(out);
        };

        // Set the position of the actor
        /// @param pos The new position of the actor
        void SetPosition(const Vec2<float> &pos);
        // Set the position of the actor
        /// @returns const Vec2
        Vec2<float> GetPosition() const;

        // Set the rotation of the actor
        /// @param rot The new rotation of the actor
        void SetRotation(float rot);
        // Get the rotation of the actor
        /// @returns float rotation
        float GetRotation() const;

        // Set the scale of the actor
        /// @param scale The new scale of the actor
        void SetScale(const Vec2<float> &scale);
        // Get the scale of the actor
        /// @returns const Vec2
        Vec2<float> GetScale() const;

        // Set the color of the actor
        /// @param color The new color of the actor
        void SetColor(const Color &color);
        // Get the color of the actor
        /// @returns const Color
        Color GetColor() const;

        // Get the sprite of the actor
        /// @returns Sprite*
        class Sprite *GetSprite() const { return mSprite; }

    protected:
        friend class Component;
        // Add a component to the actor
        void AddComponent(class Component *c);

        Sprite *mSprite = nullptr;

        Vec2<float> mPosition{0, 0};
        float mRotation{0};
        Vec2<float> mScale{1, 1};
        Color mColor{1, 1, 1};
    };

    /// @brief A PhysicalActor is a VisualActor that has a physical representation, including velocity, acceleration, collider, and mass.
    class PhysicalActor : public VisualActor
    {
    protected:
        friend class Component;

        Vec2<float> mVelocity{0, 0};
        Vec2<float> mAcceleration{0, 0};
        float mMass{1};
    };
};