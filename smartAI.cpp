//PLAY
/*
->  IF !checkIfUserExistsById(currentTarget) DEFAULT -1;
->  select random target (store currentTarget)
--->  Select random attack coord => while(!lastHit(currentTarget) {generateRandomCoord}) 
--->  Repeat until successful hit (store lastHitCoord)
----->  if(lastHitCoord) => randomly select coord surrounding hit location (x+1, x-1, y+1, y-1)
*/