{
  'targets': [
    {
      'target_name': 'nanodbc',
      'type': 'static_library',
      'sources': [
        'nanodbc.cpp'
      ],
      'include_dirs': [
        'include/'
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          'include/'
        ]
      }
    }
  ]
}