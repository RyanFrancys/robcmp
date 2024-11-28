#include <string.h>
#include <stdio.h>

void return_and_sleep(int  result) {
    volatile int a=result;
    // As part of the AVR GCC calling convention,
    // result argument (first arg) is passed in r24 and r25
    // disable interruptions and sleep; this is used to quit simavr in our tests
    asm("cli");//desabilita todos os tipos de interrupções
    asm("sleep");// dorme
}

int main() {
    
    const char *str1 = "򐟎빉萦%ˍ켁̿Ǩ윊c升T{奀վuA񲁒𫾡𤃔򜄼􋉦괄񆔷ӌ>r󮤋ֆ۲𻍠ᚧ속腠󷼨񀐶񈪾ʴ󛐯mɁ姎-蓥񒡳^OC񉡎Łȵ́ꫢ؝񎯀𯏍є򬎧6􁜧Ǚ􂸫Æ񝽥Ǆ흃򼵾ͥp񷰥񌕀y홋޹ӑqꥈ麧º겪𷣟󽨏񮆠Ҳߜ蟩̐󒉂m󽮁Ԏ粈#Aܳ𓖎k碽󶪩È೦繯˯󺶌ׂԠ:㮹>𸜝I񯙼V𙾌қ𷂯j𖪜٦Aॼ񩵜Ȯ怄᤟4񄥳肁睈)Ѿ又怾􉿿笚ǦO匯ؤĒ径lӪ񈦦񍿅ƣ꒥Yܐܩ>ꂛ߉򏆍ۗ/껠D䑥궖􌠬ՙ𓎪zb �롏|ʸh毕_􅪃0փ򈔵񤁯ܡ歨򣍓򱤂ԛ񧞂댙ڞ#򯚭򽊤ĭעǃүK񷼝�e񙖰˵𤘖ق֩R𷄹Θx-껇ǹĻ񽀄靺O㊖Ĥ󙿗򗮡!ܒOLp񽿚ݰ񢛏$搜򯝠纱؟4ر� Ὧi󴢼啣򙤙瑉⤄񫝛񛎗UϘ󩧜݋󉏿񙥠ޔҴ󨮣޳ל򓁓d􍩶򡲵𥉐9Е󇆦󀨑2gڷ^󎽠𺹽ܨՋf酁ֲ🄒^񫨅)4L񆔜�⾎𑨂򡟸R쯐Ʃ󏅡4i䀠ݾ ܫ󷡚!컳䭾fX݃Ѝ)𑳛k􋫷J閇앳񑻻@nƝ󟧈򟏞򬁶𯘗񔳅ӧd򇾾ئ򱫪î׻9⿥m˒:ȞΆ텍Ⓐ򱭴䤁𒻯身摏c󤊋Т,׉߰淜Q켮L{ĭ˨󇿛㻒ș󪲑󔗩򀵃̂ŝCN򱢕἖­𚣿񉚘ô.弎Ь:*ܽ񴷎^󁒸􈥡)ፌ杻È[Ό󘵘ᆅ眔񟑤";

    const char *substr1 = "􂸫Æ񝽥Ǆ흃򼵾ͥp񷰥񌕀y홋޹ӑqꥈ麧º겪𷣟󽨏񮆠Ҳߜ蟩̐󒉂m󽮁Ԏ粈#Aܳ𓖎k碽󶪩È೦繯˯󺶌ׂԠ:㮹>𸜝I񯙼V𙾌қ𷂯j𖪜٦Aॼ񩵜Ȯ怄᤟4񄥳肁睈)Ѿ又怾􉿿笚ǦO匯ؤĒ径lӪ񈦦񍿅ƣ꒥Yܐܩ>ꂛ߉򏆍ۗ/껠D䑥궖􌠬ՙ𓎪zb �롏|ʸh毕_􅪃0փ򈔵񤁯ܡ歨򣍓򱤂ԛ񧞂댙ڞ#򯚭򽊤ĭעǃүK񷼝�e񙖰˵𤘖ق֩R𷄹Θx-껇ǹĻ񽀄靺O㊖Ĥ󙿗򗮡!ܒOLp񽿚ݰ񢛏$搜򯝠纱؟4ر� Ὧi󴢼啣򙤙瑉⤄񫝛񛎗UϘ󩧜݋󉏿񙥠ޔҴ󨮣޳";

    const char *substr2 = "׾΁A?Ӥ󨨎r񮷤򩟴Zࢥ񸃞Σ퟽陊ޤ上櫄򼔄걜㟰żLޏu󍘧vۺ򣚃ӓ𞆟܀�􇶦IĽ栿u𮔏c􊙾𽻪퀊歲𦢠y󇎹ꉃӁ繻縠𷈶0򙤁𧭼=ԫ店򳜳͗Ȕ ƕ퇍蘛ラ𡭑 뢛4ږR첻/򇞆gI𘍎뢼􀑠8͕hȞ󝆬󷜇䯗ӹ󥺞𩏠p1ɍ𫔎*�عw٘ڞQ򝼄mr㛙唅x䞰䋝eF񲯫꿽ѭھ矩D磀Y[Pԫ򓚦ⱗ򔧕񲸤B֦툫񣼙􎺍MMl癔㨥󝰼󪰪򳓶𸰁VΈ،Y˕j׶꬇񉥵걇򓮡򘮖ᆧέ㯼ӷ􈗮󧖂򐳂BtN󞢊.䓉=󗍒ۤ𡱭�遦o񢣾؎Ьﶴ򏍋󗚇񈸝砅J񙍅恴뷀Ɠꤋ֡ٚя$";

    const char *result1 = strstr(str1, substr1);
    const char *result2 = strstr(str1, substr2);

    if (result1 != NULL && result2 == NULL) {
        return_and_sleep(0);
    } else {
        return_and_sleep(1);
    }
}