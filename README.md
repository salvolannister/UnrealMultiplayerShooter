# Unreal Multiplayer Shooter Game

Exercise to show ability in coding based in adding features to the base template from unreal "Shooter
Game" example project downloadable for free from the Epic Games Launcher.

# DONE
- Jetpack:
You can hold "J" to use the jetpack until the fuel showed by the green bar is expired
- Teleport:
You can press "T" to teleport yourself 10 m haed
- Drop Weapon:
When the player dies it will drop the weapon that could be collected by other players. The gun dropped
is the same the player was using with its current loaded and not loaded clips.
- Shrink Weapon:
When a player is hit by a rocket of the shrink weapon. They shrinks and if someone stomp on they, they dies.

# TODO

- Add overlay for shrinking
- The gun seems not to auto destroy if no one takes it
- Don't send teleport location, but calculate it from both server and user side (this strategy reduces lag)
- Set physics to falling when using Jetpack
- Strange behaviour when you press J while landing 
- Make teleport considering collision and using raycast. Consider the strange behaviour that one has when teleporting in stairs
- [IMPROVEMENT] Save mymovement component in a variable inside My_ShooterCharacter so 
that I won't need to cast it every time player asks for it
https://youtu.be/Of8SGBa3WvU?t=340
- [IMPROVEMENT] Implement client prediction for dropped gun


# USEFULL MATERIAL

- How to implement jetpack: https://www.youtube.com/watch?v=DoZyH86n_gs
- Networking and unreal saved mode: https://www.youtube.com/watch?v=Of8SGBa3WvU
- To debug network connection prediction launch in the output log: "> p.NetShowCorrections 1"
- Jetpack example implementation https://github.com/HarryPearce/JetpackTutorial
- Tips on networking https://schmigel.medium.com/unreal-engine-multiplayer-tips-c10a0c2efe40
