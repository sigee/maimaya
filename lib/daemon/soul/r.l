(raise
 *((adverbs     ,quickly,slowly,quizicaly,silently,loudly,with a pitch fork,
                backwards,sideways,cleverly,surprisingly,playfully,
                like someone I can't think of,unnoticeably,macarbly,
                scepticaly,your hopes and a family)
   (undirected
    *((mymsg    [You raise your eyebrows $ADJ])
      (theirmsg [$TP_NAME raises $TP_POSS eyebrows $ADJ])))))
$$
(ribbit
  *((noargs
  *((mymsg      [You ribbit musically])
    (theirmsg   [$TP_NAME ribbits musically])))
   (directed
    *((mymsg    [You ribbit musically at $OB_NAME])
       (theirmsg[$TP_NAME ribbits musically at $OB_NAME]))))) 
$$
(rub
 *((undirected
    *((adverbs  hands together,eyes,tummy,nose,leg,arm,eyebrows,head)
      (mymsg    [You rub your $ADJ])
      (theirmsg [$TP_NAME rubs $TP_POSS $ADJ])))
   (directed
    *((mymsg    [You rub $OB_NAME the wrong way])
      (theirmsg [$TP_NAME rubs $OB_NAME up the wrong way])))))
$$
(roar
  *((noargs
    *((mymsg    [You roar ferociously])
      (theirmsg [$TP_NAME roars ferociously])))
    (directed
     *((mysmsg  [You roar ferociously at $OB_NAME])
       (theirmsg[$TP_NAME roars ferociously at $OB_NAME])))))
$$
(roll
 *((noargs
    *((mymsg    [You roll your eyes])
      (theirmsg [$TP_NAME rolls $TP_POSS eyes])))
   (undirected
    *((adverbs  your eyes,around on the floor laughing,over and play dead,
                up into a ball,your own cigarette,sideways,backwards,
                profesionaly,like a spy through an imaginary door,fearfully,
                tightly,slowly)
      (theirmsg [$TP_NAME rolls $ADJ])
      (mymsg    [You roll $ADJ]))
   (directed
    *((adverbs  ,quickly,slowly,painfully,excitededly,lovingly,carefully)
      (theirmsg [$TP_NAME rolls $ADJ over $OB_NAME])
      (mymsg    [You roll $ADJ over $OB_NAME])))))
$$
(ruffle
 *((adverbs     ,playfully,affectionately)
   (directed
    *((theirmsg [$TP_NAME ruffles $OB_NPOSS hair $ADJ])
      (mymsg    [You ruffle $OB_NPOSS hair $ADJ]))))



