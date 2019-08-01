#include "jsmn.h"

#include "structs.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "treatment.h"
#include "test.h"

Graph g;
typedef enum {
  NUL, FROM, TO, LENGTH, OBJ, ENDOBJ, PATH, ARRAY, ON, OFF, TMAX
} last_string;
last_string last;
int period;
int tmax;

/* Function realloc_it() is a wrapper function for standard realloc()
 * with one difference - it frees old memory pointer in case of realloc
 * failure. Thus, DO NOT use old data pointer in anyway after call to
 * realloc_it(). If your code has some kind of fallback algorithm if
 * memory can't be re-allocated - use standard realloc() instead.
 */
static inline void *realloc_it(void *ptrmem, size_t size) {
  void *p = realloc(ptrmem, size);
  if (!p) {
    free(ptrmem);
    fprintf(stderr, "realloc(): errno=%d\n", errno);
  }
  return p;
}

/*
 * An example of reading JSON from stdin and printing its content to stdout.
 * The output looks like YAML, but I'm not sure if it's really compatible.
 */

static int dump(const char *js, jsmntok_t *t, size_t count, int indent) {
  int i, j, k;
  jsmntok_t *key;
  char str[64];
  int val;
  if (count == 0) {
    return 0;
  }
  if (t->type == JSMN_PRIMITIVE) {
    sprintf(str,"%.*s", t->end - t->start, js + t->start);
    val = atoi(str);
    switch(last){
      case FROM:
        g.arc_pool[g.arc_pool_size-1].first = val;
      break;
      case TO:
        g.arc_pool[g.arc_pool_size-1].last = val;
      break;
      case LENGTH:
        g.arc_pool[g.arc_pool_size-1].length = val;
      break;
      default:
          perror("This should not happend.\n ");
          exit(26);

    }
    return 1;
    
  } else if (t->type == JSMN_STRING) {
      sprintf(str,"%.*s", t->end - t->start, js + t->start);
      if(!strcmp(str,"from"))
      {
        last = FROM;
        return 1;
      }
      if(!strcmp(str,"to"))
      {
        last = TO;
        return 1;
      }
      if(!strcmp(str,"length"))
      {
        last = LENGTH;
        return 1;
      }
      if(last == OBJ)
        return 1;
      if(last == ENDOBJ)
        return 1;
     
      fprintf(stderr,"VARIABLE %s not known in arc.\n ",str);
      exit(26);
 
  } else if (t->type == JSMN_OBJECT) {
    if(last!= NUL)
    {
      g.arc_pool_size++;

        g.arc_pool = realloc(g.arc_pool,sizeof(Arc)*g.arc_pool_size);
      g.arc_pool[g.arc_pool_size-1].nb_routes = 0;
      
    }
    last = OBJ;
    j = 0;
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent; k++) {
      }
      key = t + 1 + j;
      j += dump(js, key, count - j, indent + 1);
      if (key->size > 0) {
        j += dump(js, t + 1 + j, count - j, indent + 1);
      }
    }
    last = ENDOBJ;
    return j + 1;
  } else if (t->type == JSMN_ARRAY) {
    j = 0;
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent - 1; k++) {
      }
      j += dump(js, t + 1 + j, count - j, indent + 1);
    }
    return j + 1;
  }
  return 0;
}


static int dumppath(const char *js, jsmntok_t *t, size_t count, int indent) {
  int i, j, k;
  jsmntok_t *key;
  char str[64];
  int val;
  if (count == 0) {
    return 0;
  }
  if (t->type == JSMN_PRIMITIVE) {
    sprintf(str,"%.*s", t->end - t->start, js + t->start);
    val = atoi(str);
    
    if(last == ARRAY)
    {
      g.size_routes[g.nb_routes-1]++;
      g.routes[g.nb_routes-1] = realloc(g.routes[g.nb_routes-1],sizeof(Arc*)* g.size_routes[g.nb_routes-1]);
      g.routes[g.nb_routes-1][g.size_routes[g.nb_routes-1]-1] = &g.arc_pool[val];
      
      g.arc_pool[val].routes_id[g.arc_pool[val].nb_routes] = g.nb_routes-1;
      g.arc_pool[val].nb_routes++;
    
      return 1;
    }
    perror("This should not happend.\n ");
    exit(26);
    
  } else if (t->type == JSMN_STRING) {
      sprintf(str,"%.*s", t->end - t->start, js + t->start);
      if(last == OBJ)
        return 1;
      if(last == ENDOBJ)
        return 1;
      if(!strcmp(str,"path"))
      {
        last = PATH;
        return 1;
      }
      
      fprintf(stderr,"VARIABLE %s not known in arc.\n ",str);
      exit(26);
 
  } else if (t->type == JSMN_OBJECT) {
    if(last!= NUL)
    {
      g.nb_routes++;
      g.routes = realloc(g.routes,sizeof(Route)*g.nb_routes);
      g.size_routes = realloc(g.size_routes,sizeof(int)*g.nb_routes);
      g.routes[g.nb_routes-1] = NULL;
      g.size_routes[g.nb_routes-1]=0;
    }
    last = OBJ;
    j = 0;
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent; k++) {
      }
      key = t + 1 + j;
      j += dumppath(js, key, count - j, indent + 1);
      if (key->size > 0) {
        j += dumppath(js, t + 1 + j, count - j, indent + 1);
      }
    }
    last = ENDOBJ;
    return j + 1;
  } else if (t->type == JSMN_ARRAY) {
    j = 0;
    last = ARRAY;
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent - 1; k++) {
      }
      j += dumppath(js, t + 1 + j, count - j, indent + 1);
    }
    last = ENDOBJ;
    return j + 1;
  }
  return 0;
}

static int dumpflows(const char *js, jsmntok_t *t, size_t count, int indent) {
  int i, j, k;
  static int on_once = 0;
  static int off_once = 0;

  jsmntok_t *key;
  char str[64];
  int val;
  if (count == 0) {
    return 0;
  }
  if (t->type == JSMN_PRIMITIVE) {
    sprintf(str,"%.*s", t->end - t->start, js + t->start);
    val = atoi(str);
    
    switch(last){
      case ON:
        if(!on_once)
        {
          period += val;
          on_once = 1;
        }
      break;
      case OFF:
        if(!off_once)
        {
          period += val;
          off_once = 1;
        }
      break;
      case TMAX:
        tmax = val;
      break;
      default:
        return 1;
    }
    return 1;
    
  } else if (t->type == JSMN_STRING) {
      sprintf(str,"%.*s", t->end - t->start, js + t->start);
      if(!strcmp(str,"on"))
      {
        last = ON;
        
      }
      if(!strcmp(str,"off"))
      {
        last = OFF;
        
      }
      if(!strcmp(str,"deadline"))
      {
        last = TMAX;
        
      }
      return 1;
 
  } else if (t->type == JSMN_OBJECT) {
    
    j = 0;
    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent; k++) {
      }
      key = t + 1 + j;
      j += dumpflows(js, key, count - j, indent + 1);
      if (key->size > 0) {
        j += dumpflows(js, t + 1 + j, count - j, indent + 1);
      }
    }

    return j + 1;
  } else if (t->type == JSMN_ARRAY) {
    j = 0;

    for (i = 0; i < t->size; i++) {
      for (k = 0; k < indent - 1; k++) {
      }
      j += dumpflows(js, t + 1 + j, count - j, indent + 1);
    }
    return j + 1;
  }
  return 0;
}

int parsearcs() {
  int r;
  int eof_expected = 0;
  char *js = NULL;
  size_t jslen = 0;
  char buf[BUFSIZ];
  FILE* file;
  jsmn_parser p;
  jsmntok_t *tok;
  size_t tokcount = 2;


  last = NUL;
  char* filearcs = "../../onos/Topolexample/links.json";


  /* Prepare parser */
  jsmn_init(&p);
  file = fopen(filearcs,"r");
  if(!file)
  {
    fprintf(stderr,"Error opening %s .\n",filearcs);
    return EXIT_FAILURE;
  }

  /* Allocate some tokens as a start */
  fprintf(stdout, "Reading %s......\n",filearcs);

  tok = malloc(sizeof(*tok) * tokcount);
  if (tok == NULL) {
    fprintf(stderr, "malloc(): errno=%d\n", errno);
    return 3;
  }

  for (;;) {
    /* Read another chunk */
    r = fread(buf, 1, sizeof(buf), file);
    if (r < 0) {
      fprintf(stderr, "fread(): %d, errno=%d\n", r, errno);
      return 1;
    }
    if (r == 0) {
      if (eof_expected != 0) {
        break;
      } else {
        fprintf(stderr, "fread(): unexpected EOF\n");
        return 2;
      }
    }

    js = realloc_it(js, jslen + r + 1);
    if (js == NULL) {
      return 3;
    }
    strncpy(js + jslen, buf, r);
    jslen = jslen + r;

  again:
    r = jsmn_parse(&p, js, jslen, tok, tokcount);
    if (r < 0) {
      if (r == JSMN_ERROR_NOMEM) {
        tokcount = tokcount * 2;
        tok = realloc_it(tok, sizeof(*tok) * tokcount);
        if (tok == NULL) {
          return 3;
        }
        goto again;
      }
    } else {
      dump(js, tok, p.toknext, 0);
      eof_expected = 1;
    }
  }

  fclose(file);
  free(tok);
  free(js);
  fprintf(stdout, "Ok.\n");
  for(int i=0;i<g.arc_pool_size;i++)
  {
    printf("Arc %p: from %d to %d length %d \n",&g.arc_pool[i],g.arc_pool[i].first,g.arc_pool[i].last,g.arc_pool[i].length);
  }
  return 1;
}
int parsepath()
{
  int r;
  int eof_expected = 0;
  char *js = NULL;
  size_t jslen = 0;
  char buf[BUFSIZ];
  FILE* file;
  jsmn_parser p;
  jsmntok_t *tok;
  size_t tokcount = 2;


  last = NUL;
  char* filepath = "../../onos/Topolexample/path.json";


  jsmn_init(&p);
  file = fopen(filepath,"r");
  if(!file)
  {
    fprintf(stderr,"Error opening %s .\n",filepath);
    return EXIT_FAILURE;
  }

  /* Allocate some tokens as a start */
  fprintf(stdout, "Reading %s......\n",filepath);

  tok = malloc(sizeof(*tok) * tokcount);
  if (tok == NULL) {
    fprintf(stderr, "malloc(): errno=%d\n", errno);
    return 3;
  }

  for (;;) {
    /* Read another chunk */
    r = fread(buf, 1, sizeof(buf), file);
    if (r < 0) {
      fprintf(stderr, "fread(): %d, errno=%d\n", r, errno);
      return 1;
    }
    if (r == 0) {
      if (eof_expected != 0) {
        break;
      } else {
        fprintf(stderr, "fread(): unexpected EOF\n");
        return 2;
      }
    }

    js = realloc_it(js, jslen + r + 1);
    if (js == NULL) {
      return 3;
    }
    strncpy(js + jslen, buf, r);
    jslen = jslen + r;

  againpath:
    r = jsmn_parse(&p, js, jslen, tok, tokcount);
    if (r < 0) {
      if (r == JSMN_ERROR_NOMEM) {
        tokcount = tokcount * 2;
        tok = realloc_it(tok, sizeof(*tok) * tokcount);
        if (tok == NULL) {
          return 3;
        }
        goto againpath;
      }
    } else {
      dumppath(js, tok, p.toknext, 0);
      eof_expected = 1;
    }
  }
  fclose(file);
  free(tok);
  free(js);

  for(int i=0;i<g.nb_routes;i++)
  {
    for(int j=0;j<g.size_routes[i];j++)
    {
        g.routes[i][j]->bbu_dest = g.routes[i][g.size_routes[i]-1]->last;
    }
  }
  fprintf(stdout, "Ok.\n");
    printf("%d routes : \n",g.nb_routes);
  for(int i=0;i<g.nb_routes;i++)
  {
    printf("route of size %d : \n",g.size_routes[i]);
    for(int j=0;j<g.size_routes[i];j++)
    {
      printf("%d - ",g.routes[i][j]->length);
    }
    printf("\n");
  }

   
  return 1;
}

int parseflows()
{
  int r;
  int eof_expected = 0;
  char *js = NULL;
  size_t jslen = 0;
  char buf[BUFSIZ];
  FILE* file;
  jsmn_parser p;
  jsmntok_t *tok;
  size_t tokcount = 2;


  last = NUL;
  char* filepath = "../../onos/Topolexample/flows.json";


  jsmn_init(&p);
  file = fopen(filepath,"r");
  if(!file)
  {
    fprintf(stderr,"Error opening %s .\n",filepath);
    return EXIT_FAILURE;
  }

  /* Allocate some tokens as a start */
  fprintf(stdout, "Reading %s......\n",filepath);

  tok = malloc(sizeof(*tok) * tokcount);
  if (tok == NULL) {
    fprintf(stderr, "malloc(): errno=%d\n", errno);
    return 3;
  }

  for (;;) {
    /* Read another chunk */
    r = fread(buf, 1, sizeof(buf), file);
    if (r < 0) {
      fprintf(stderr, "fread(): %d, errno=%d\n", r, errno);
      return 1;
    }
    if (r == 0) {
      if (eof_expected != 0) {
        break;
      } else {
        fprintf(stderr, "fread(): unexpected EOF\n");
        return 2;
      }
    }

    js = realloc_it(js, jslen + r + 1);
    if (js == NULL) {
      return 3;
    }
    strncpy(js + jslen, buf, r);
    jslen = jslen + r;

  againflows:
    r = jsmn_parse(&p, js, jslen, tok, tokcount);
    if (r < 0) {
      if (r == JSMN_ERROR_NOMEM) {
        tokcount = tokcount * 2;
        tok = realloc_it(tok, sizeof(*tok) * tokcount);
        if (tok == NULL) {
          return 3;
        }
        goto againflows;
      }
    } else {
      dumpflows(js, tok, p.toknext, 0);
      eof_expected = 1;
    }
  }
  fclose(file);
  free(tok);
  free(js);
  printf("OK.\n");
   
  return 1;
}
/*
void count_cols_bbu(Graph g,int* p_bbu,int * p_cols)
{
  for(int i=0;i<g.arc_pool_size;i++)
  {
    g.arc_pool[i].seen = 0;
  }
  for(int i=0;i<g.nb_routes;i++)
  {
    for(int j=1;j<g.size_routes[i];j++)
    {
      if(g.routes[i][j]->seen == 0)
      {
        if(g.routes[i][j-1]->nb_routes < g.routes[i][j]->nb_routes )
        {
          if(j == g.size_routes[i]-1)
          {
            *p_bbu++;
          }
          else
          {
            *p_cols++;
          }
        }
        g.routes[i][j]->seen = 1;
      }
    }
  }

}*/
void jsontest()
{
  g.arc_pool = NULL;
  g.routes = NULL;
  g.size_routes = NULL;
  g.arc_pool_size = 0;
  period = 0;
  tmax = 0;
  parsearcs();
  parsepath();  
  parseflows();
 // int nb_bbu=0;
 // int nb_cols=0;
  for(int i=0;i<g.arc_pool_size;i++)
  {
    g.arc_pool[i].period_f = calloc(period,sizeof(int));
    g.arc_pool[i].period_b = calloc(period,sizeof(int));
  }
 // count_cols_bbu(g,&nb_bbu,&nb_cols);
//  printf("%d %d \n",nb_bbu,nb_cols);
  printf("%d %d \n",period,tmax);
  affiche_graph(g,period,stdout);
  free_graph(g);
}
