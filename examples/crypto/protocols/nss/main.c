#include "nss.h"
#include "attacker.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*@
predicate_family_instance pthread_run_pre(attacker_t)(void *data, any info) = 
    exists(nss_pub) &*& [_]world(nss_pub) &*& 
    attacker_proof_obligations(nss_pub) &*&
    initial_principals() &*& !bad(0) &*&
    info == nil;
@*/

void *attacker_t(void* _unused) //@ : pthread_run_joinable
  //@ requires pthread_run_pre(attacker_t)(_unused, _);
  //@ ensures false;
{
  //@ open pthread_run_pre(attacker_t)(_unused, _);
  attacker();
  return 0;
}

struct nss_args
{
  int sender;
  int receiver;
  struct item *key_AS;
  struct item *key_BS;
  struct item *key_AB;
};

/*@
predicate_family_instance pthread_run_pre(server_t)(void *data, any info) = 
  [_]world(nss_pub) &*&
  nss_args_sender(data, ?sender) &*&
  nss_args_receiver(data, ?receiver) &*&
  nss_args_key_AS(data, ?key_AS) &*&
  nss_args_key_BS(data, ?key_BS) &*&
  nss_args_key_AB(data, ?key_AB) &*&
  !bad(0) &*& !bad(sender) &*& !bad(receiver) &*&
  key_item(key_AS, sender, 0, symmetric_key, int_pair(0, 0)) &*&
  key_item(key_BS, receiver, 0, symmetric_key, int_pair(0, 0)) &*&
  key_item(key_AB, 0, ?id, symmetric_key, int_pair(2, int_pair(sender, receiver))) &*&
  info == nil;
predicate_family_instance pthread_run_post(server_t)(void *data, any info) = 
  [_]world(nss_pub) &*&
  nss_args_sender(data, ?sender) &*&
  nss_args_receiver(data, ?receiver) &*&
  nss_args_key_AS(data, ?key_AS) &*&
  nss_args_key_BS(data, ?key_BS) &*&
  nss_args_key_AB(data, ?key_AB) &*&
  info == nil;
@*/

void *server_t(void* data) //@ : pthread_run_joinable
  //@ requires pthread_run_pre(server_t)(data, ?x);
  //@ ensures pthread_run_post(server_t)(data, x) &*& result == 0;
{
  struct nss_args *args = data;
  //@ open pthread_run_pre(server_t)(data, _);
  //@ open key_item(args->key_AB, _, _, _, _);
  server(args->sender, args->receiver, args->key_AS, args->key_BS, args->key_AB);
  key_free(args->key_AS);
  key_free(args->key_BS);
  item_free(args->key_AB);
  //@ close pthread_run_post(server_t)(data, _);
  return 0;
}

/*@
predicate_family_instance pthread_run_pre(receiver_t)(void *data, any info) = 
  [_]world(nss_pub) &*&
  nss_args_receiver(data, ?receiver) &*&
  nss_args_key_BS(data, ?key_BS) &*&
  !bad(0) &*& !bad(receiver) &*&
  generated_nonces(receiver, _) &*&
  key_item(key_BS, receiver, 0, symmetric_key, int_pair(0, 0)) &*&
  info == cons(receiver, nil);
predicate_family_instance pthread_run_post(receiver_t)(void *data, any info) = 
  [_]world(nss_pub) &*&
  nss_args_receiver(data, ?receiver) &*&
  nss_args_key_BS(data, ?key_BS) &*&
  generated_nonces(receiver, _) &*&
  info == cons(receiver, nil);
@*/

void *receiver_t(void* data) //@ : pthread_run_joinable
  //@ requires pthread_run_pre(receiver_t)(data, ?x);
  //@ ensures pthread_run_post(receiver_t)(data, x) &*& result == 0;
{
  struct nss_args *args = data;
  //@ open pthread_run_pre(receiver_t)(data, _);
  receiver(args->receiver, args->key_BS);
  key_free(args->key_BS);
  //@ close pthread_run_post(receiver_t)(data, _);
  return 0;
}

/*@
predicate_family_instance pthread_run_pre(sender_t)(void *data, any info) = 
  [_]world(nss_pub) &*&
  nss_args_sender(data, ?sender) &*&
  nss_args_receiver(data, ?receiver) &*&
  nss_args_key_AS(data, ?key_AS) &*&
  !bad(0) &*& !bad(sender) &*& !bad(receiver) &*&
  generated_nonces(sender, _) &*&
  key_item(key_AS, sender, 0, symmetric_key, int_pair(0, 0)) &*&
  info == cons(sender, nil);
predicate_family_instance pthread_run_post(sender_t)(void *data, any info) = 
  [_]world(nss_pub) &*&
  nss_args_sender(data, ?sender) &*&
  nss_args_receiver(data, ?receiver) &*&
  nss_args_key_AS(data, ?key_AS) &*&
  generated_nonces(sender, _) &*&
  info == cons(sender, nil);
@*/

void *sender_t(void* data) //@ : pthread_run_joinable
  //@ requires pthread_run_pre(sender_t)(data, ?x);
  //@ ensures pthread_run_post(sender_t)(data, x) &*& result == 0;
{
  struct nss_args *args = data;
  //@ open pthread_run_pre(sender_t)(data, _);
  struct item *result = sender(args->sender, args->receiver, args->key_AS);
  key_free(result);
  key_free(args->key_AS);
  //@ close pthread_run_post(sender_t)(data, _);
  return 0;
}

int main()
  /*@ requires generated_keys(0, 0) &*& net_api_uninitialized() &*& 
               initial_principals(); 
  @*/
  //@ ensures true;
{
  struct item *key_AS;
  struct item *key_BS;
  struct item *key_AB;
  struct keypair *keypair;

  printf("\n\tExecuting \"nss protocol\" ... ");
  
  //@ init_protocol();
  //@ assert protocol_pub(?pub);
  //@ close exists(pub);
  init_crypto_lib();
  
  //@ open initial_principals();
  int sender = create_principal(&key_AS, &keypair);
  keypair_free(keypair);
  int receiver = create_principal(&key_BS, &keypair);
  keypair_free(keypair);
  //@ close key_request(0, int_pair(2, int_pair(sender, receiver))); 
  key_AB = create_key();
  //@ close initial_principals();
  
  //@ assume (!bad(0) && !bad(sender) && !bad(receiver));
  void *null = (void *) 0;
  
  {
    pthread_t a_thread;
    //@ PACK_ATTACKER_PROOF_OBLIGATIONS(nss)
    //@ close attacker_proof_obligations(nss_pub);
    //@ leak  world(nss_pub);
    //@ close pthread_run_pre(attacker_t)(null, _);
    pthread_create(&a_thread, null, &attacker_t, null);  
  }
  
  int i = 0;
#ifdef EXECUTE
  while (i++ < 10)
#else
  while (true)
#endif
    /*@ invariant 
              [_]world(nss_pub) &*&
              generated_nonces(sender, _) &*&
              key_AS |-> ?kkey_AS &*& key_item(kkey_AS, 
                          sender, 0, symmetric_key, int_pair(0, 0)) &*&
              generated_nonces(receiver, _) &*&
              key_BS |-> ?kkey_BS &*& key_item(kkey_BS, 
                          receiver, 0, symmetric_key, int_pair(0, 0)) &*&
              key_item(key_AB, 0, _, symmetric_key, int_pair(2, int_pair(sender, receiver)));
    @*/
  { 
    pthread_t serv_thread, send_thread, rec_thread;
    struct nss_args serv_args, send_args, rec_args;
    {
      serv_args.sender = sender;
      serv_args.receiver = receiver;
      serv_args.key_AS = key_clone(key_AS);
      serv_args.key_BS = key_clone(key_BS);
      serv_args.key_AB = key_clone(key_AB);
      //@ close pthread_run_pre(server_t)(&serv_args, _);
      pthread_create(&serv_thread, null, &server_t, &serv_args); 
      
      rec_args.receiver = receiver;
      rec_args.key_BS = key_clone(key_BS);
      //@ close pthread_run_pre(receiver_t)(&rec_args, cons(receiver, nil));
      pthread_create(&rec_thread, null, &receiver_t, &rec_args);
      
      send_args.sender = sender;
      send_args.receiver = receiver;
      send_args.key_AS = key_clone(key_AS);
      //@ close pthread_run_pre(sender_t)(&send_args, cons(sender, nil));
      pthread_create(&send_thread, null, &sender_t, &send_args); 
    }
    
    {
      pthread_join(serv_thread, null);
      //@ open pthread_run_post(server_t)(&serv_args, _);
      pthread_join(rec_thread, null);
      //@ open pthread_run_post(receiver_t)(&rec_args, _); 
      pthread_join(send_thread, null);
      //@ open pthread_run_post(sender_t)(&send_args, _);
    }
  }
  
  printf("Done\n");
}