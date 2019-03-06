#_init and _fini should be local
VERS_1.0 {
  global:
    initonnxruntime_pybind11_state;    

  # Hide everything else.
  local:
    *;
};
