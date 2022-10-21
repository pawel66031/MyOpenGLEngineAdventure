#ifndef STATES_HPP
#define STATES_HPP

namespace States {
    // Check target state
    template<typename T>
    bool isIndexActive(T* states, int target){
       return (*states & (1 << target)) == (1 << target); 
    }

    // Activate target state
    template<typename T>
    void activateIndex(T* states, int target) {
        *states |= 1 << target;
    }

    // Uniquely activate target index (no other can be active)
    template<typename T>
    void uniquelyActivateIndex(T* states, int target) {
        activateIndex<T>(states, target); // Activate state first
        *states &= 1 << target; // Deactivate others
    }

    // Deactivate target index
    template<typename T>
    void deactivateIndex(T* states, int target){
        *states &= ~(1 << target);
    }

    // Toggle target index
    template<typename T>
    void toggleIndex(T* states, int target){
        *states ^= 1 << target;
    }

    // Check state
    template<typename T>
    bool isActive(T* states, T state){
        return (*states & state) == state;
    }

    // Activate state
    template<typename T>
    void activate(T* states, T state){
        *states |= state;
    }

    // Uniquely activate state (no other can be active)
    template<typename T>
    void uniquelyActivate(T* states, T state) {
        *states &= ~state;
    }

    // Deactivate state
    template<typename T>
    void deactivate(T* states, T state){
        *states &= ~state;
    }

    // Toggle state
    template<typename T>
    void toggle(T* states, T state){
        *states ^= state;
    }
}

#endif //STATES_HPP