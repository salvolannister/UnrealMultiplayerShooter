# unreal-shooter-game-ex-one
Exercise to show ability in coding based in adding features to the base template from unreal " ....".
Features (networked) added by now:
- Jetpack:
You can hold "J" to use the jetpack until the fuel showed by the green bar is expired
- Teleport:
You can press "T" to teleport yourself 10 m haed
- Drop Weapon:
When the player dies it will drop the weapon that could be collected by other players. The gun dropped
is the same the player was using with its current loaded and not loaded clips.
- Shrink Weapon:
When a player is hit by a rocket of the shrink weapon. They shrinks and if someone stomp on they, they dies.

#TODO

- Aggiungere overlay dello shrinking
- L'arma sembra non autodistruggersi se nessuno la prende
- Non mandare la teleport location, ma calcolarla sia in loco che in remoto (questa strategia è preferibile a causa del lag)
- Settare la fisica a falling quando si usa il jetpack
- Comportamento strano quando si preme il tasto del jetpack appena si sta atterrando
- Fare il teleport considerando le collisioni e usando un raycast per trovare i muri dinnanzi a se. Sistemare il comportamento strano che
si ha quando ci si teleporta nelle scale
- [IMPROVEMENT] Save mymovement component in a variable inside My_ShooterCharacter so 
that I won't need to cast it every time I ask for it
https://youtu.be/Of8SGBa3WvU?t=340
- [IMPROVEMENT] Implement client prediction for dropped gun



#OF NOTICE
Sto usando they al posto di he/she al singolare 



#USEFULL MATERIAL

- video implementing jetpack: https://www.youtube.com/watch?v=DoZyH86n_gs
- video explaining networking in unreal and saved move: https://www.youtube.com/watch?v=Of8SGBa3WvU
- To debug network connection prediction launch in the output log: "> p.NetShowCorrections 1"
- github containing interesting code https://github.com/HarryPearce/JetpackTutorial
- tips on networking https://schmigel.medium.com/unreal-engine-multiplayer-tips-c10a0c2efe40
