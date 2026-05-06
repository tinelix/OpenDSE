#ifdef UNIX

#ifndef UNIX_LEGACY

__attribute__((constructor))
void libcrocon_start() {

}

__attribute__((destructor))
void libcrocon_finish() {

}

#endif

#endif
